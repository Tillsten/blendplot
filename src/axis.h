#include <vector>
#include <functional>
#include <array>
#include "./common.h"
#include "canvas.h"
#include "artists.h"

class Axis
{
private:
    const Canvas& canvas;
    void draw_self(BLContext& ctx);
    void update_transforms();
public:
    Simple::Signal<void(double, double)> sizeChanged;
    BLRgba32 facecolor = BLRgba32(0xFFFFFFFF);
    BLRgba32 linecolor = BLRgba32(0xFF000000);
    bool grid_x_enabeled = true;
    bool grid_y_enabeled = false;
    BLMatrix2D inch2data;
    BLMatrix2D data2inch;
    BLMatrix2D datax2inch;
    BLMatrix2D datay2inch;
    BLRect position;
    std::vector<std::shared_ptr<Artist>> artists = {};
    BLBox view_lim = BLBox(0, 0, 10, 10);
    void draw(BLContext& ctx);
    void draw_ticks(BLContext& ctx);
    void draw_text_path(BLContext& ctx, BLPath& path, BLPoint& px_pos, TextHA ha = TextHA::left, TextVA va = TextVA::bottom);
    void canvas_resized(double w, double h);

    Axis(Canvas& canvas) : canvas(canvas) {                            
        position.reset(0.1, 0.1, 0.8, 0.8);
        canvas.sizeChanged.connect(Simple::slot(this, &Axis::canvas_resized));          
        canvas.drawRequested.connect(Simple::slot(this, &Axis::draw));
        update_transforms();
    };
};

void Axis::update_transforms()
{
    auto xs = view_lim.x1 - view_lim.x0;
    auto ys = view_lim.y1 - view_lim.y0;

    inch2data.resetToTranslation(position.x, position.y);
    inch2data.scale(position.w / xs, position.h / ys);
    inch2data.translate(-view_lim.x0, -view_lim.y0);
    BLMatrix2D::invert(data2inch, inch2data);

    datax2inch = make_blended(inch2data, canvas.pixel2inch);
    datay2inch = make_blended(canvas.pixel2inch, inch2data);
};

void Axis::draw(BLContext& ctx){
    
    ctx.save();
    draw_self(ctx);
    ctx.clipToRect(position);
    ctx.transform(inch2data);    
    for (auto &&i : artists)
    {
        ctx.save();
        i->draw(ctx);
        ctx.restore();
    };
    ctx.restore();
};

void Axis::draw_self(BLContext& ctx)
{
    ctx.save();    
    ctx.setFillStyle(facecolor);
    ctx.setStrokeStyle(linecolor);
    ctx.strokeRect(position);
    //ctx.translate(position.x, position.y);
    draw_ticks(ctx);
    ctx.restore();
};


void Axis::canvas_resized(double w, double h) {
    position.reset(0.3, 0.3, w - 1.5*0.3,  h-1.5*0.3);
    update_transforms();
}

    
std::vector<double> calculate_ticks(const double left, const double right, const double size, std::optional<int> num_ticks = std::nullopt) {
    using namespace std;

    std::vector<double> out{};
    double min = std::min(left, right);
    double max = std::max(left, right);
    double range = max - min;

    if (!isfinite(range)) {
        return out;
    }
    num_ticks = num_ticks.value_or(floor(size * 2));
    double exponent = log10(abs(range));
    double magnitude = pow(10, floor(exponent) - 1);
    double right_scaled = floor(max / magnitude);
    double left_scaled = ceil(min / magnitude);
    const std::array<double, 5> steps = { 1, 2, 2.5, 5, 10 };
    int step_ticks;
    double step_taken = NAN;
    for (const double& s : steps) {
        step_ticks = ceil((right_scaled - left_scaled) / s);
        if (step_ticks <= num_ticks) {
            step_taken = s;
            break;
        }
    }

    if (step_taken == NAN) { return out; };

    for (int i = 0; i <= step_ticks; i++)
    {   
        double tick = left_scaled * magnitude + i * magnitude * step_taken;
        if (tick < max) {
            out.push_back(left_scaled * magnitude + i * magnitude * step_taken);
        }
        else {
            break;
        }

    }
    return out;
}

wstring inline format_tick(const double t) {
    wchar_t buf[50];
    swprintf(buf, 50, L"%.1f", t);
    return wstring(buf);
}

void Axis::draw_ticks(BLContext& ctx) {
    auto xticks = calculate_ticks(view_lim.x0, view_lim.x1, position.w);
    auto yticks = calculate_ticks(view_lim.y0, view_lim.y1, position.h);
    double tick_length = 0.05/2.;
    ctx.translate(0, position.y);
    for (auto& t : xticks)
    {        
        auto pos = BLPoint(t, 0);
        auto spos = inch2data.mapPoint(pos);          
        if (grid_x_enabeled) {            
            ctx.setStrokeAlpha(0.5);
            ctx.strokeLine(spos.x, 0, spos.x, position.h);
            ctx.setStrokeAlpha(1.);
        }
        ctx.strokeLine(spos.x, -tick_length, spos.x, tick_length);                
        auto path = font_manager.std_font.get_path(format_tick(t));        
        auto px_pos = ctx.userMatrix().mapPoint(BLPoint(spos.x, - tick_length - 0.05));
        draw_text_path(ctx, path, px_pos, TextHA::center, TextVA::top);        

    }

    ctx.translate(position.x, -position.y);
    for (auto& t : yticks)
    {       
        auto pos = BLPoint(0, t);
        auto spos = inch2data.mapPoint(pos);
        ctx.strokeLine(-tick_length, spos.y, tick_length, spos.y);
        auto path = font_manager.std_font.get_path(format_tick(t));
        auto px_pos = ctx.userMatrix().mapPoint(BLPoint(-tick_length - 0.05, spos.y));
        draw_text_path(ctx, path, px_pos, TextHA::right, TextVA::center);
    }
}


void Axis::draw_text_path(BLContext& ctx, BLPath& path, BLPoint& px_pos, TextHA ha, TextVA va)
{
    ctx.save();
    ctx.resetMatrix();
    BLBox bbox;
    path.getControlBox(&bbox);
    BLPoint shift = BLPoint(0, 0);
    double w = bbox.x1 - bbox.x0;
    double h = bbox.y1 - bbox.y0;
    
    switch (va)
    {
    case TextVA::bottom:
        shift.y = 0;
        break;
    case TextVA::center:
        shift.y = h/ 2.;
        break;
    case TextVA::top:
        shift.y = h;
        break;
    default:
        break;
    }

    switch (ha)
    {
    case TextHA::left:
        shift.x = 0;
        break;
    case TextHA::center:
        shift.x = w / 2;
        break;
    case TextHA::right:
        shift.x = w;
        break;
    }

    ctx.translate(round(px_pos.x - shift.x), round(px_pos.y + shift.y));
    ctx.setFillStyle(linecolor);
    ctx.fillPath(path);
    
    ctx.restore();
}

//def get_ticks(limits : Tuple[float, float], width : float)->List[float]:
//"""
//Generates tick position for a linear axis
//
//Parameters
//----------
//limits : (float, float)
//View limits
//width : float
//Width in inches.
//"""
//limits = min(limits), max(limits)
//max_ticks = floor(width * 3)
//total = limits[1] - limits[0]
//if not isfinite(total) :
//    return[]
//    magitude = floor(log10(abs(total)))
//
//    steps = [1, 2, 5, 10]
//    decade = 10 * *(magitude - 1)
//    for s in steps :
//step = s * decade
//n_ticks = total / step
//if n_ticks <= max_ticks :
//    break
//    start = ceil(limits[0] / step)
//    end = floor(limits[1] // step)
//        return[i * step for i in range(start, end + 1)]
//

