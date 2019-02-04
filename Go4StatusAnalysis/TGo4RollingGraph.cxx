#include "TGo4RollingGraph.h"

#include "TMath.h"

#include <cstring>

/** \brief Default constructor
*
* Creates a rolling graph with an update interval of 1, i.e. no averaging of the input values
* Graph will have no size, either. Without setting size, nothing will be drawn!
*/
TGo4RollingGraph::TGo4RollingGraph() :
TGraphErrors(),
fiNumPoints(0),
fiUpdateInterval(1),
fiCounter(0),
fdSum(0.),
fdSumAveraged(0.)
{
}

/** \brief Constructor with size and update interval
* \param NumPoints Size of the graph, i.e. number of points
* \param UpdateInterval No. of calls to the Fill() routine after which a new bin will started to be filled. For UpdateInterval=1 every value will be displayed directly on the rolling graph, for values > 1 an average will be shown
*/
TGo4RollingGraph::TGo4RollingGraph(Int_t NumPoints, Int_t UpdateInterval):
TGraphErrors(),
fiNumPoints(NumPoints),
fiUpdateInterval(UpdateInterval),
fiCounter(0),
fdSum(0.),
fdSumAveraged(0.)
{
}

/** \brief Destructor */
TGo4RollingGraph::~TGo4RollingGraph()
{
}

/** \brief Fills a new value into the rolling graph
* \param value The value to fill
*
* If the Update Interval is larger than one, the value will not be displayed directly in the graph but added to the averaging
*/
void TGo4RollingGraph::Fill(Double_t value, Double_t xerror, Double_t yerror)
{
	if (fiNumPoints)
	{
		Int_t NumAverage = fiCounter % fiUpdateInterval + 1;
		// Create a new bin and reset internal value for averaging if counter is a multiple of fiUpdateInterval
		if (NumAverage == 1)
		{
			NextBin();
			fdSum = 0.;
		}

		// Add value for averaging
		fdSum += value;
		fdSumAveraged = fdSum / static_cast<Double_t>(NumAverage);

		// Draw new averaged value
		SetPoint(fNpoints-1, static_cast<Double_t>(fiCounter / fiUpdateInterval), fdSumAveraged);
		if (xerror < 0)
			xerror = 0.5;
		if (yerror < 0)
			yerror = fdSumAveraged / TMath::Sqrt(NumAverage);
		SetPointError(fNpoints-1, xerror, yerror);
	}
	fiCounter++;
}

/** \brief Create a new bin 
*
* Internal function that implements the "rolling" of the graph: The first bin is deleted while a new one is appended at the end
*/
void TGo4RollingGraph::NextBin()
{
	if (fiNumPoints)
	{
		if (fiNumPoints <= fNpoints)
		{
			// Move points to the left
			memmove (&fX[0],  &fX[1],  (fNpoints-1)*sizeof(Double_t));
			memmove (&fEX[0], &fEX[1], (fNpoints-1)*sizeof(Double_t));
			memmove (&fY[0],  &fY[1],  (fNpoints-1)*sizeof(Double_t));
			memmove (&fEY[0], &fEY[1], (fNpoints-1)*sizeof(Double_t));
		}
		else
		{
			// Create a new point
			Set(fNpoints+1);
		}
	}
}

/** \brief Clear all points
*
* Remove all points from the graph, set number of points to zero, and reset internal value for averaging
*/
void TGo4RollingGraph::Clear()
{
	for (Int_t i=0; i<fiNumPoints; ++i)
		RemovePoint(i);

	Set(0);
	fdSum = 0.;
}


ClassImp(TGo4RollingGraph);
