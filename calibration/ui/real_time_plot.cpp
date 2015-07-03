#include "real_time_plot.h"
#include "utils.h"


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cRealTimePlot::cRealTimePlot(const std::string& title, const std::string& xAxisLabel,
							 const std::string& yAxisLabel,
							 const std::string& line1Legend, const std::string& line2Legend = "",
							 const std::string& line3Legend = "", const std::string& line4Legend = "")
	: mPlotStream( 1, 1, 255, 255, 255, "xcairo" )
{
	// --- Set Up Plotting --- //
	
	PLFLT ymin = 0.0, ymax = 1.0;
	PLFLT xmin = 0.0, xmax = 200.0, xjump_pct = 0.1;

	PLINT colbox = 1, collab = 3;

	// Line styles - solid
	PLINT styline[4];
	styline[0] = 1;
	styline[1] = 1;
	styline[2] = 1;
	styline[3] = 1;

	// Pen colors
	PLINT colline[4];
	colline[0] = nUtils::enumPLplotColor_RED;
	if( !line2Legend.empty() )
	{
		++mNumActivePlots;
		colline[1] = nUtils::enumPLplotColor_GREEN;
	}
	else
	{
		colline[1] = nUtils::enumPLplotColor_WHITE;
	}

	if( !line3Legend.empty() )
	{
		++mNumActivePlots;
		colline[2] = nUtils::enumPLplotColor_BLUE;
	}
	else
	{
		colline[2] = nUtils::enumPLplotColor_WHITE;
	}

	if( !line4Legend.empty() )
	{
		++mNumActivePlots;
		colline[3] = nUtils::enumPLplotColor_MAGENTA;
	}
	else
	{
		colline[4] = nUtils::enumPLplotColor_WHITE;
	}

	// Strings for plot legend
	const char* legline[4];
	legline[0] = line1Legend.c_str();
	legline[1] = line2Legend.c_str();
	legline[2] = line3Legend.c_str();
	legline[3] = line4Legend.c_str();

	PLFLT xlab = 0.0, ylab = 0.25;

	bool autoy = true, acc = false;

	pls.init();

	pls.adv(0);
	pls.vsta();

	pls.stripc( &mPlotId, "bcnst", "bcnstv",
				xmin, xmax, xjump_pct, ymin, ymax,
				xlab, ylab,
				autoy, acc,
				colbox, collab,
				colline, styline, legline,
				xAxisLabel.c_str(), yAxisLabel.c_str(), title.c_str() );


	// --- Set Up Thread-Related Stuff --- //

	mContinuePlotting = true;
	mBufferedPoints.clear();
	pthread_mutex_init(&mDataMutex, NULL);
	pthread_create(&mPlottingThreadHandle, NULL, ThreadFunc, (void*)this);
}

////////////////////////////////////////////////////////////////////////////////

cRealTimePlot::~cRealTimePlot()
{
	mContinuePlotting = false;
	pthread_join(mPlottingThreadHandle, NULL);
	pthread_mutex_destroy(&mDataMutex);
	
	mPlotStream.stripd( mPlotId );
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::AddDataPoint(double pt)
{
	EnqueueDataPoint(pt, pt, pt, pt);
}

////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::AddDataPoint(double pt1, double pt2)
{
	EnqueueDataPoint(pt1, pt2, pt2, pt2);
}

////////////////////////////////////////////////////////////////////////////////
	
void cRealTimePlot::AddDataPoint(double pt1, double pt2, double pt3)
{
	EnqueueDataPoint(pt1, pt2, pt3, pt3);
}

////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::AddDataPoint(double pt1, double pt2, double pt3, double pt4)
{
	EnqueueDataPoint(pt1, pt2, pt3, pt4);
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::EnqueueDataPoint(double p1, double p2, double p3, double p4)
{
	sDataPoint p;
	p.mPoint1 = p1;
	p.mPoint2 = p2;
	p.mPoint3 = p3;
	p.mPoint4 = p4;

	pthread_mutex_lock(&mDataMutex);
	mBufferedPoints.push_back(p);
	pthread_mutex_unlock(&mDataMutex);
}

////////////////////////////////////////////////////////////////////////////////
//  Threaded Functions
////////////////////////////////////////////////////////////////////////////////

void* cRealTimePlot::ThreadFunc(void* pIn)
{
	cRealTimePlot* pThis = reinterpret_cast<cRealTimePlot*>(pIn);

	sDataPoint p;
	bool plot = false;
	int pointNum = 0;
	while(mContinuePlotting)
	{
		// Try to grab the point.
		pthread_mutex_lock(&mDataMutex);
		if( !mBufferedPoints.empty() )
		{
			p = mBufferedPoints.front();
			mBufferedPoints.pop_front();
			plot = true;
		}
		pthread_mutex_unlock(&mDataMutex);

		// Plot it.
		if( plot )
		{
			pThis->mPlotStream.stripa( mPlotId, 0, pointNum, p.mPoint1 );
			pThis->mPlotStream.stripa( mPlotId, 1, pointNum, p.mPoint2 );
			pThis->mPlotStream.stripa( mPlotId, 2, pointNum, p.mPoint3 );
			pThis->mPlotStream.stripa( mPlotId, 3, pointNum, p.mPoint4 );
			++pointNum;
			plot = false;
		}
	}
}
