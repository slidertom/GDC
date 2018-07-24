#include "stdafx.h"
#include "svgGDC.h"

#include "../GDC.h" 

#include "memory"
#include "fstream"

#include "Collections/MxKMacros.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

class CSvgFileAbs
{
// Construction/Desctruction
public:
    CSvgFileAbs() { }
    virtual ~CSvgFileAbs() { }

// Overrides
public:
    virtual void write(const char *sBuffer) = 0;
};

class CSvgFile : public CSvgFileAbs
{
// Construction/Desctruction
public:
    CSvgFile(const wchar_t *sFilePath) {
        m_pFile = new std::ofstream;
        m_pFile->open(sFilePath);
        #ifdef _DEBUG // currently only for the debug
        m_sFileName = sFilePath;
        wchar_t drive[_MAX_DRIVE];
	    wchar_t dir[_MAX_DIR];
	    wchar_t fname[_MAX_FNAME];
	    wchar_t ext[_MAX_EXT];
	    ::_wsplitpath(m_sFileName.c_str(), drive, dir, fname, ext);
        if ( ::wcscmp(ext, L".html") == 0) {
            *m_pFile << "<html>";
        }
        #endif
    }
    virtual ~CSvgFile() {
        #ifdef _DEBUG // currently only for the debug
        wchar_t drive[_MAX_DRIVE];
	    wchar_t dir[_MAX_DIR];
	    wchar_t fname[_MAX_FNAME];
	    wchar_t ext[_MAX_EXT];
	    ::_wsplitpath(m_sFileName.c_str(), drive, dir, fname, ext);
        if ( ::wcscmp(ext, L".html") == 0) {
            *m_pFile << "</html>";
        }
        #endif

        m_pFile->flush();
        m_pFile->close();
        delete m_pFile;
    }

// Overrides
public:
    virtual void write(const char *sBuffer) override {
        *m_pFile << sBuffer;
    }

// Attributes
private:
    #ifdef _DEBUG 
    std::wstring m_sFileName;
    #endif
    std::ofstream *m_pFile;
};

class CSvgBuffer : public CSvgFileAbs
{
// Construction/Desctruction
public:
    CSvgBuffer(std::string *pBuffer) : m_pBuffer(pBuffer) { }
    ~CSvgBuffer() { }

// Overrides
public:
    virtual void write(const char *sBuffer) override {
        *m_pBuffer += sBuffer;
    }

// Attributes
private:
    std::string *m_pBuffer;
};

SvgGDC::SvgGDC(const wchar_t *sFilePath, int32_t nWidth, int32_t nHeight)
{
    m_pFile = new CSvgFile(sFilePath);
    
    m_nWidth  = nWidth;
    m_nHeight = nHeight;
}

SvgGDC::SvgGDC(std::string *pBuffer, int32_t nWidth, int32_t nHeight)
{
    m_pFile = new CSvgBuffer(pBuffer);
    m_nWidth  = nWidth;
    m_nHeight = nHeight;
}

SvgGDC::~SvgGDC()
{
    line("</svg>");
    delete m_pFile;
}

void SvgGDC::line(const char *val1) {
    m_pFile->write(val1);
    m_pFile->write(enter_elem());
}
void SvgGDC::line(const char *val1, const char *val2, const char *val3) {
    m_pFile->write(val1);
    m_pFile->write(val2);
    m_pFile->write(val3);
    m_pFile->write(enter_elem());
}
void SvgGDC::line(const char *val1, const char *val2, const char *val3, const char *val4) {
    m_pFile->write(val1);
    m_pFile->write(val2);
    m_pFile->write(val3);
    m_pFile->write(val4);
    m_pFile->write(enter_elem());
}
void SvgGDC::line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5) {
    m_pFile->write(val1);
    m_pFile->write(val2);
    m_pFile->write(val3);
    m_pFile->write(val4);
    m_pFile->write(val5);
    m_pFile->write(enter_elem());
}
void SvgGDC::line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5, const char *val6) {
    m_pFile->write(val1);
    m_pFile->write(val2);
    m_pFile->write(val3);
    m_pFile->write(val4);
    m_pFile->write(val5);
    m_pFile->write(val6);
    m_pFile->write(enter_elem());
}
void SvgGDC::line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5, const char *val6, const char *val7) {
    m_pFile->write(val1);
    m_pFile->write(val2);
    m_pFile->write(val3);
    m_pFile->write(val4);
    m_pFile->write(val5);
    m_pFile->write(val6);
    m_pFile->write(val7);
    m_pFile->write(enter_elem());
}
void SvgGDC::line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5, const char *val6, const char *val7, const char *val8) {
    m_pFile->write(val1);
    m_pFile->write(val2);
    m_pFile->write(val3);
    m_pFile->write(val4);
    m_pFile->write(val5);
    m_pFile->write(val6);
    m_pFile->write(val7);
    m_pFile->write(val8);
    m_pFile->write(enter_elem());
}
void SvgGDC::line(const char *val1, const char *val2, const char *val3, const char *val4, const char *val5, const char *val6, const char *val7, const char *val8, const char *val9) {
    m_pFile->write(val1);
    m_pFile->write(val2);
    m_pFile->write(val3);
    m_pFile->write(val4);
    m_pFile->write(val5);
    m_pFile->write(val6);
    m_pFile->write(val7);
    m_pFile->write(val8);
    m_pFile->write(val9);
    m_pFile->write(enter_elem());
}

static inline std::string ColorToString(int32_t r, int32_t g, int32_t b)
{
    std::string str  = "rgb(";
                str += std::to_string(r);
                str += ",";
                str += std::to_string(g);
                str += ",";
                str += std::to_string(b);
                str += ")";
    return str;
}

static inline std::string ColorToString(COLORREF color)
{
    int32_t r = GetRValue(color);
    int32_t g = GetGValue(color);
    int32_t b = GetBValue(color);
    return ColorToString(r, g, b);
}

static inline std::string ColorToStringRGBA(int32_t r, int32_t g, int32_t b, int32_t a)
{
    std::string str  = "rgba(";
                str += std::to_string(r);
                str += ",";
                str += std::to_string(g);
                str += ",";
                str += std::to_string(b);
                str += ",";
                str += std::to_string(a);
                str += ")";
    return str;
}

static inline std::string PaintColorToString(const GDCPaint &paint)
{
    int32_t alfa = paint.GetAlfa();
    if ( alfa == -1 ) {
        return ColorToString(paint.GetColor());
    }
    else {
        const COLORREF color = paint.GetColor();
        int32_t r = GetRValue(color);
        int32_t g = GetGValue(color);
        int32_t b = GetBValue(color);
        return ColorToStringRGBA(r, g, b, alfa);
    }
}

static inline std::string StrokeColor(const GDCPaint &paint)
{
    std::string str  = "stroke:";
                str += PaintColorToString(paint);
    return str;
}
// skia/include/core/SkString.h
// Floats have at most 8 significant digits, so we limit our %g to that.
// However, the total string could be 15 characters: -1.2345678e-005
//
//  In theory we should only expect up to 2 digits for the exponent, but on
//  some platforms we have seen 3.
#define SkStrAppendScalar_MaxSize  15
static char* SkStrAppendFloat(char string[], float value) {
    // SKIA: since floats have at most 8 significant digits, we limit our %g to that.
    // https://stackoverflow.com/questions/12815179/number-of-significant-digits-for-a-floating-point-type
    // according to the standard, not all decimal number can be stored exactly in memory. 
    // Depending on the size of the representation, the error can get to a certain maximum. 
    // For float this is 0.0001% (6 significant digits = 10^-6 = 10^-4 %).
    // e.g: 0.85 -> results: 0.85000002 with the %.8g vs2017
    static const char gFormat[] = "%.6g";
    // make it 1 larger for the terminating 0
    char buffer[SkStrAppendScalar_MaxSize + 1];
    auto len = ::snprintf(buffer, sizeof(buffer), gFormat, value);
    memcpy(string, buffer, len);
    ASSERT(len <= SkStrAppendScalar_MaxSize);
    return string + len;
}
static inline std::string float_to_string(float fValue)
{
    char  buffer[SkStrAppendScalar_MaxSize];
    char *stop = SkStrAppendFloat(buffer, fValue);
    const std::string sValue(buffer, stop - buffer);
    return sValue;
}

static inline std::string StrokeWidth(const GDCPaint &paint)
{
    std::string str;

    float strokeWidth = paint.GetStrokeWidth();
    
    if (strokeWidth == 0) { // SKIA version
        // Hairline stroke
        // https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/vector-effect
        // The value "non-scaling-stroke" is designed so that it can be implemented 
        // without the entire vector effect engine. For example, profiles of SVG may 
        // restrict the values of vector-effect to be "default" or "non-scaling-stroke". 
        // In effect this requires no processing of vector effects, rather it is always 
        // the default rendering order with a different set of transformations.
        strokeWidth = 1.f;
        //str += "vector-effect:non-scaling-stroke;";   
        // ISSUE 31 Values of vector-effect other than non-scaling-stroke and none are at risk of being dropped from SVG 2 
        // due to a lack of implementations. Feedback from implementers is requested, regarding the practicality of implementing 
        // them as currently specified, during the implementation period. 
    }
    else if (strokeWidth < 0.85f) { // if lines are not visible -> please try to disable all external css -> could be an issue with the css 
        strokeWidth = 0.85f; // due the antialiasing: crispEdges everything bellow ~0.81px is not visible with the 100% zoom
    }

    // https://css-tricks.com/almanac/properties/s/stroke-width/    
    // stroke-width: 2px
    // stroke-width: 2em
    // stroke-width: 2
    // stroke-width: 2.5
    // stroke-width: 15%
    str += "stroke-width:";     
    str += float_to_string(strokeWidth);
    //str += "px"; // e.g.: skia uses always svg defined units 
    return str;
}

static inline std::string Stroke(const GDCPaint &paint)
{
    std::string str = StrokeColor(paint);
                str  += ";";
                str  += StrokeWidth(paint);

    // https://www.w3schools.com/graphics/tryit.asp?filename=trysvg_stroke3
    const GDCStrokeType stroke = paint.GetStrokeType();
    switch (stroke)
    {
    case GDC_PS_SOLID:
        break;
    case GDC_PS_DASH:
        {
            str += ";";
            str += "stroke-dasharray:20,10";
        }
        break;
    case GDC_PS_DOT:
        {
            str += ";";
            str += "stroke-dasharray:5,5";
        }
        break;
    case GDC_PS_DASHDOT:
        {
            str += ";";
            str += "stroke-dasharray:20,5,5,5"; // stroke-length, space-length, stroke-length, ...
        }
        break;
	case GDC_PS_DASHDOTDOT:
		{
            str += ";";
            str += "stroke-dasharray:20,5,5,5,5,5"; // stroke-length, space-length, stroke-length, ...
        }
        break;
    }
    return str;
}

inline static std::string CreatePatern(const char *sColor, const char *sPaternName, const char *sPaternPath)
{
    std::string sValue = "<defs>";
        sValue += "<pattern id=\""; sValue += sPaternName; sValue += "\" patternUnits=\"userSpaceOnUse\" width=\"10\" height=\"10\">";
			sValue += "<svg width='10' height='10'>";
			sValue += "<rect width='10' height='10' fill='none'/>";
				sValue += "<path d='";  sValue += sPaternPath; sValue += "' stroke='"; sValue += sColor;  sValue += "' stroke-width='1'/> ";
			sValue += "</svg>";
        sValue += "</pattern>";
    sValue += "</defs>";
    return sValue;
}

inline static std::string CreateBDiagPatern(const char *sColor, const char *sPaternName)
{
	return CreatePatern(sColor, sPaternName, "M-1,1 l2,-2 M0,10 l10,-10 M9,11 l2,-2");
}

inline static std::string CreateFDiagPatern(const char *sColor, const char *sPaternName)
{
	return CreatePatern(sColor, sPaternName, "M9,-1 l2,2 M0,0 l10,10 M-1,9 l2,2");
}

inline static std::string CreateDiagCrossPatern(const char *sColor, const char *sPaternName)
{
	return CreatePatern(sColor, sPaternName, "M0 0L10 10ZM10 0L0 10Z");
}

inline static std::string CreateHorzPatern(const char *sColor, const char *sPaternName)
{
	return CreatePatern(sColor, sPaternName, "M0 1L10 1");
}

inline static std::string CreateVertPatern(const char *sColor, const char *sPaternName)
{
	return CreatePatern(sColor, sPaternName, "M1 0L1 10");
}

inline static std::string CreateCrossPatern(const char *sColor, const char *sPaternName)
{
	return CreatePatern(sColor, sPaternName, "M0 1L10 1M1 0L1 10");
}

static inline std::string FillColor(const GDCPaint &paint)
{
    std::string str  = "fill:";
                str += PaintColorToString(paint);
    return str;
}

static inline std::string FillPattern(const char *sPatternId)
{
    std::string str  = "fill:url(#";
                str += sPatternId;
				str += ")";
    return str;
}

std::string SvgGDC::GetPattern(const GDCPaint &fill_paint)
{
	const GDCPaintType eFill = fill_paint.GetPaintType();

	if (eFill <= GDC_FILL) {
		return "";
	}

	const COLORREF color = fill_paint.GetColor();

	//Patterns with different colors support
	std::string sPatternName = std::to_string(eFill);
	sPatternName += '-';
	sPatternName += std::to_string(color);

	auto it_find = std::find(m_patterns.begin(), m_patterns.end(), sPatternName);
	if (m_patterns.end() != it_find) {
		return sPatternName;
	}

	const std::string sColor = ColorToString(color);
	std::string sPatternDef;
	//If pattern doesn't exist - create and write to file
	switch (eFill)
	{
	case GDC_FILL_DIAGCROSS:
		{
			sPatternDef = CreateDiagCrossPatern(sColor.c_str(), sPatternName.c_str());
		}
		break;
	case GDC_FILL_HORIZONTAL:
		{
			sPatternDef = CreateHorzPatern(sColor.c_str(), sPatternName.c_str());
		}
		break;
	case GDC_FILL_VERTICAL:
		{
			sPatternDef = CreateVertPatern(sColor.c_str(), sPatternName.c_str());
		}
		break;
	case GDC_FILL_FDIAGONAL:
		{
			sPatternDef = CreateFDiagPatern(sColor.c_str(), sPatternName.c_str());
		}
		break;
	case GDC_FILL_BDIAGONAL:
		{
			sPatternDef = CreateBDiagPatern(sColor.c_str(), sPatternName.c_str());
		}
		break;
	case GDC_FILL_CROSS:
		{
			sPatternDef = CreateCrossPatern(sColor.c_str(), sPatternName.c_str());
		}
		break;
	default:
		return "";
		break;
	}

	m_patterns.push_back(sPatternName);
	line(sPatternDef.c_str());

	return sPatternName;
}

void SvgGDC::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) 
{
     std::string sStyle   = "\" style=\"";
                 sStyle  += Stroke(paint);
                 sStyle  += "\" />";

     line("<line x1=\"", std::to_string(x1).c_str(),
             "\" y1=\"", std::to_string(y1).c_str(),
             "\" x2=\"", std::to_string(x2).c_str(), 
             "\" y2=\"", std::to_string(y2).c_str(), 
             sStyle.c_str());
}

void SvgGDC::DrawPoint(int32_t x, int32_t y, const GDCPaint &paint) 
{
    const std::string sRad = std::to_string(paint.GetStrokeWidth());
    const std::string sFill = FillColor(paint);
    line("<circle cx=\"", std::to_string(x).c_str(), "\" cy=\"", std::to_string(y).c_str(), "\" r=\"", sRad.c_str(), "\" style=\"", sFill.c_str(), "\"/>");
}

static inline std::string PointToStr(const GDCPoint &pt) {
    std::string str  = std::to_string(pt.x);
                str += ",";
                str += std::to_string(pt.y);
    return str;
}

static inline std::string PointsToStr(const std::vector<GDCPoint> &points) {
    std::string str = "\"";
    for (const auto &pt : points) {
        str += PointToStr(pt);
        str += " ";
    }
    str += "\"";
    return str;
}

std::string SvgGDC::GetFill(const GDCPaint &fill_paint)
{
	const std::string sPatternName = GetPattern(fill_paint);
	std::string sFill;
	if (!sPatternName.empty()) {
		sFill = FillPattern(sPatternName.c_str());
	}
	else {
		sFill = FillColor(fill_paint);
	}

	return sFill;
}

void SvgGDC::DrawPolygon(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint, const GDCPaint &stroke_paint) 
{
	const std::string sFill   = GetFill(fill_paint);
    const std::string sPoints = PointsToStr(points);
    line("<polygon points=", sPoints.c_str(), " style=\"", sFill.c_str(), ";", Stroke(stroke_paint).c_str(), "\" />");
}

void SvgGDC::DrawPoly(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint) 
{
    std::vector<GDCPoint> points2 = points;
    points2.push_back(points.front());
    DrawPolyLine(points2, stroke_paint);
}

void SvgGDC::DrawPolyLine(const std::vector<GDCPoint> &points, const GDCPaint &stroke_paint) 
{
    std::string sPoints = PointsToStr(points);
    line("<polyline points=", sPoints.c_str(), " style=\"fill:none;", Stroke(stroke_paint).c_str(), "\" />");
}
    
void SvgGDC::DrawPolygonTransparent(const std::vector<GDCPoint> &points, const GDCPaint &fill_paint) 
{
    ASSERT(FALSE); // implementation is missing - TODO
}

inline static std::string CreateHorizontalGradient(const char *sColorFrom, const char *sColorTo, const char *sGradId)
{
    std::string sValue = "<defs>";
        sValue += "<linearGradient id=\""; sValue += sGradId; sValue += "\" x1=\"0%\" y1=\"0%\" x2=\"100%\" y2=\"0%\">";
        sValue += "<stop offset=\"0%\" style=\"stop-color:"; sValue += sColorFrom; sValue += ";stop-opacity:1\" />";
        sValue += "<stop offset=\"100%\" style=\"stop-color:", sValue += sColorTo; sValue += ";stop-opacity:1\" />";
        sValue += "</linearGradient>";
    sValue += "</defs>";
    return sValue;
}

void SvgGDC::DrawPolygonGradient(const std::vector<GDCPoint> &points, const GDCPaint &paintFrom, const GDCPaint &paintTo) 
{
    const COLORREF color_from = paintFrom.GetColor();
    const COLORREF color_to   = paintTo.GetColor();
    
    const std::string sColorFrom = ColorToString(color_from);
    const std::string sColorTo   = ColorToString(color_to);

    std::string sGradId;

    std::string sGradColor = sColorFrom + sColorTo;
    auto found = m_horizontal_grads.find(sGradColor);
    if ( found == m_horizontal_grads.end() ) {
        sGradId = "grad";
        sGradId += std::to_string(m_horizontal_grads.size() + 1);
        const std::string sGradient = ::CreateHorizontalGradient(sColorFrom.c_str(), sColorTo.c_str(), sGradId.c_str()); // do generate only once
        line(sGradient.c_str());
        m_horizontal_grads[sGradColor] = sGradId.c_str();
    }
    else {
        sGradId = found->second;
    }
    
    std::string sPoints = PointsToStr(points);
    line("<polygon points=", sPoints.c_str(), " style=\"fill:url(#", sGradId.c_str(), ")", "\" />");
}

void SvgGDC::DrawFilledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &fill_paint) 
{
    ASSERT(FALSE);  // implementation is missing - TODO
}

void SvgGDC::DrawRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) 
{
    std::string sStyle  = "style=\"fill:none;";
                sStyle += Stroke(paint).c_str();

    const int32_t nWidth  = ::abs(x2 - x1);
    const int32_t nHeight = ::abs(y2 - y1);
    std::string sWidth  = "width=\"";
                sWidth += std::to_string(nHeight).c_str();
                sWidth += "\"";
    std::string sHeight  = "height=\"";
                sHeight += std::to_string(nHeight).c_str();
                sHeight += "\"";

    std::string sX  = "x=\"";
                sX += std::to_string(x1);
                sX += "\"";
    std::string sY = "y=\"";
                sY += std::to_string(y1);
                sY += "\"";
    std::string sXY = sX + " " + sY;

    line("<rect ", sXY.c_str(), " ", sWidth.c_str(), " ", sHeight.c_str(), " ", sStyle.c_str(), "\" />");
}

void SvgGDC::DrawEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint)
{
	const int32_t radius_x = int32_t(::abs(x2 - x1) * 0.5);
	const int32_t radius_y = int32_t(::abs(y2 - y1) * 0.5);

	const int32_t center_x = int32_t((x1 + x2) * 0.5);
	const int32_t center_y = int32_t((y1 + y2) * 0.5);

	std::string sLine = "<ellipse cx=\"";
	sLine += std::to_string(center_x) + "\" cy=\"";
	sLine += std::to_string(center_y) + "\" rx=\"";
	sLine += std::to_string(radius_x) + "\" ry=\"";
	sLine += std::to_string(radius_y) + "\"";
	sLine += " style=\"fill:none;" + Stroke(paint);
	sLine += "\"/>";
	line(sLine.c_str());
}

void SvgGDC::DrawFilledEllipse(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const GDCPaint &paint) 
{
	const int32_t radius_x = int32_t(::abs(x2-x1) * 0.5);
	const int32_t radius_y = int32_t(::abs(y2-y1) * 0.5);

	const int32_t center_x = int32_t((x1 + x2) * 0.5);
	const int32_t center_y = int32_t((y1 + y2) * 0.5);

	const std::string sFill = FillColor(paint);
	std::string sLine = "<ellipse cx=\"";
	sLine += std::to_string(center_x) + "\" cy=\"";
	sLine += std::to_string(center_y) + "\" rx=\"";
	sLine += std::to_string(radius_x) + "\" ry=\"";
	sLine += std::to_string(radius_y) + "\"";
	sLine += " style="+sFill+";" + Stroke(paint);
	sLine += "\"/>";
	line(sLine.c_str());
}

void SvgGDC::DrawHollowOval(int32_t xCenter, int32_t yCenter, int32_t rx, int32_t ry, int32_t h, const GDCPaint &fill_paint)
{
    //void SkSVGDevice::drawOval(const SkRect& oval, const SkPaint& paint) {
    //AutoElement ellipse("ellipse", fWriter, fResourceBucket.get(), MxCp(this), paint);
    //ellipse.addAttribute("cx", oval.centerX());
    //ellipse.addAttribute("cy", oval.centerY());
    //ellipse.addAttribute("rx", oval.width() / 2);
    //ellipse.addAttribute("ry", oval.height() / 2);
}

void SvgGDC::DrawArc(int32_t x, int32_t y, const int32_t nRadius, const float fStartAngle, const float fSweepAngle, const GDCPaint &paint) 
{
	const int32_t x1 = int32_t(x + nRadius * (::cos(-fStartAngle * PI / 180.0)));
	const int32_t y1 = int32_t(y + nRadius * (::sin(-fStartAngle * PI / 180.0)));

	const int32_t x2 = int32_t(x + nRadius * (::cos(-fSweepAngle * PI / 180.0)));
	const int32_t y2 = int32_t(y + nRadius * (::sin(-fSweepAngle * PI / 180.0)));

	std::string sLine = "<path d=\"";
	sLine += "M" + std::to_string(x) + " " + std::to_string(y);   // Move to center
	sLine += "L" + std::to_string(x1) + " " + std::to_string(y1); // Line to first point
	sLine += "A" + std::to_string(nRadius) + " " + std::to_string(nRadius) + " 0 0 0 " + std::to_string(x2) + " " + std::to_string(y2); // Actual Arc to point nr. 2
	sLine += "L" + std::to_string(x) + " " + std::to_string(y);   // Move to center
	sLine += "\"  style=\"fill:none;" + Stroke(paint);
	sLine += "\"/>";
	line(sLine.c_str());
}	

void SvgGDC::DrawBitmap(HBITMAP hBitmap, int32_t x, int32_t y) 
{
    // https://stackoverflow.com/questions/6249664/does-svg-support-embedding-of-bitmap-images
    /*
     if (!pngData) {
        return;
    }

    size_t b64Size = SkBase64::Encode(pngData->data(), pngData->size(), nullptr);
    SkAutoTMalloc<char> b64Data(b64Size);
    SkBase64::Encode(pngData->data(), pngData->size(), b64Data.get());

    SkString svgImageData("data:image/png;base64,");
    svgImageData.append(b64Data.get(), b64Size);

    SkString imageID = fResourceBucket->addImage();
    {
        AutoElement defs("defs", fWriter);
        {
            AutoElement image("image", fWriter);
            image.addAttribute("id", imageID);
            image.addAttribute("width", bm.width());
            image.addAttribute("height", bm.height());
            image.addAttribute("xlink:href", svgImageData);
        }
    }

    {
        AutoElement imageUse("use", fWriter, fResourceBucket.get(), mc, paint);
        imageUse.addAttribute("xlink:href", SkStringPrintf("#%s", imageID.c_str()));
    }
    */
}   

// or maybe should be used:
// http://stackoverflow.com/questions/4358870/convert-wstring-to-string-encoded-in-utf-8
// std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
// return myconv.to_bytes(str);
static inline std::string ConvertToUTF8(const wchar_t *wstr)
{
    const int32_t nLen = (int32_t)wcslen(wstr);
    if ( nLen <= 0  ) {
        return std::string();
    }

    const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

void SvgGDC::TextOut(const wchar_t *sText, int32_t x, int32_t y, const GDCPaint &paint) 
{
    //int32_t angle = (int32_t)(-vector_util::CalcAngle(v) * 1800.0 / PI);
    // calculates angle clockwise from x axis to vector v
    // from -pi to pi
    // font-family
    // http://vanseodesign.com/web-design/svg-text-baseline-alignment/
    std::string sPaint;
    {
        const GDCFontDescr *pFont = paint.GetFontDescr();
        {
            std::string sColor = PaintColorToString(paint);
            std::string sStyle = "style=\"";
                        sStyle += "font-family:";
                        sStyle += ConvertToUTF8(pFont->m_sFontName.c_str());
                        sStyle += ";";
                        sStyle += "font-size:";
                        sStyle += std::to_string(::abs(pFont->m_nHeight));
                        sStyle += ";";
                        sStyle += "font-weight:";
                        sStyle += std::to_string(pFont->m_weight);
                        sStyle += ";";
                        sStyle += "fill:";
                        sStyle += sColor.c_str();
                        sStyle += ";";
                        if ( pFont->m_nTextAlign & GDC_TA_LEFT ) {
                            sStyle += "text-anchor:start";
                        }
                        else if ( pFont->m_nTextAlign & GDC_TA_CENTER ) {
                            sStyle += "text-anchor:middle";
                        }
                        else if ( pFont->m_nTextAlign & GDC_TA_RIGHT ) {
                            sStyle += "text-anchor:end";
                        }

						sStyle += ";";
						if ( pFont->m_nTextAlign & GDC_TA_BOTTOM ) {
                            sStyle += "dominant-baseline:text-after-edge";
                        }
                        else if ( pFont->m_nTextAlign & GDC_TA_BASELINE ) {
                            sStyle += "alignment-baseline:hanging";
                        }
                        else if ( pFont->m_nTextAlign & GDC_TA_TOP ) {
                            sStyle += "dominant-baseline:text-before-edge";
                        }
                        sStyle += "\"";
            sPaint  = sStyle;
        }
        {
            float fAngle = pFont->m_fAngle;
            fAngle = float((-1)*fAngle/10.);
            std::string sAngle = float_to_string(fAngle);
            std::string sTransform  = "transform=\"rotate(";
                        sTransform += sAngle;
                        sTransform += " ";
                        sTransform += std::to_string(x);
                        sTransform += ",";
                        sTransform += std::to_string(y);
                        sTransform += ")\"";
            sPaint += " ";
            sPaint += sTransform;
        }
    }

    const std::string sTextUTF8 = ConvertToUTF8(sText);

    line("<text x=\"", std::to_string(x).c_str(),
            "\" y=\"", std::to_string(y).c_str(), 
            "\" ", sPaint.c_str(), ">", sTextUTF8.c_str(), "</text>");
}

#include "../GDI/oligdi.h"

int32_t SvgGDC::GetTextHeight(const GDCPaint &paint) const
{
    OWindowDC dc(nullptr); // screen
    GDC gdc(dc.GetSafeHdc());
    return gdc.GetTextHeight(paint);
}

GDCSize SvgGDC::GetTextExtent(const wchar_t *sText, size_t nCount, const GDCPaint &paint) const
{
    OWindowDC dc(nullptr); // screen
    GDC gdc(dc.GetSafeHdc());
    return gdc.GetTextExtent(sText, paint);
}

void SvgGDC::SetViewportOrg(int32_t x, int32_t y) 
{
    const std::string sWidth  = std::to_string(m_nWidth);
    const std::string sHeight = std::to_string(m_nHeight);

    std::string sViewBox  = "viewbox=\"";
                sViewBox += std::to_string(-x);
                sViewBox += " ";
                sViewBox += std::to_string(-y);
                sViewBox += " ";
                sViewBox += std::to_string(m_nWidth);
                sViewBox += " ";
                sViewBox += std::to_string(m_nHeight);
                sViewBox += "\"";

    // https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/shape-rendering
    // *auto
    // Indicates that the user agent shall make appropriate tradeoffs to balance speed, crisp edges and geometric precision, 
    // but with geometric precision given more importance than speed and crisp edges.
    // *optimizeSpeed
    // Indicates that the user agent shall emphasize rendering speed over geometric precision and crisp edges. 
    // This option will sometimes cause the user agent to turn off shape anti-aliasing.
    // *crispEdges
    // Indicates that the user agent shall attempt to emphasize the contrast between clean edges of artwork over rendering speed and geometric precision. 
    // To achieve crisp edges, the user agent might turn off anti-aliasing for all lines and curves or possibly just for straight lines which are close 
    // to vertical or horizontal. Also, the user agent might adjust line positions and line widths to align edges with device pixels.
    // *geometricPrecision
    // Indicates that the user agent shall emphasize geometric precision over speed and crisp edges.

    line("<svg width=\"", sWidth.c_str(), "\" height=\"", sHeight.c_str(), "\" ", sViewBox.c_str(), " shape-rendering=\"crispEdges\">");
    // special case for the cromium engine: staticly types invert filter  to have at least possibility to hilight elements
    // https://stackoverflow.com/questions/32567156/why-dont-css-filters-work-on-svg-elements-in-chrome
    // workaround:
    line("<defs> <filter id=\"invert_svg\"> <feColorMatrix type=\"matrix\" \
        values='-1  0  0 0 1      \
                 0 -1  0 0 1      \
                 0  0 -1 0 1      \
                 0  0  0 0.5 0'/> \
          </filter></defs>");
}

GDCPoint SvgGDC::GetViewportOrg() const 
{ 
    return GDCPoint(0, 0);
}

HDC SvgGDC::GetHDC() { return nullptr; }

void SvgGDC::BeginGroup(const char *sGroupAttributes)
{
     line("<g ", sGroupAttributes, ">");
}

void SvgGDC::EndGroup()
{
    line("</g>");
}

void SvgGDC::DrawTextByEllipse(double dRadiusX, double dRadiusY, const GDCPoint &ptCenter, const wchar_t *sText, double dEllipseAngleRad, const GDCPaint &paint)
{
    //
}