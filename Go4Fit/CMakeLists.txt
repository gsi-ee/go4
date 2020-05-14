set(Go4Fit_headers
      TGo4FitAmplEstimation.h
      TGo4FitAxisTrans.h
      TGo4FitComponent.h
      TGo4FitDataGraph.h
      TGo4FitData.h
      TGo4FitDataHistogram.h
      TGo4FitDataProfile.h
      TGo4FitDataRidge.h
      TGo4FitDependency.h
      TGo4FitLinearTrans.h
      TGo4FitMatrixTrans.h
      TGo4FitMinuit.h
      TGo4FitMinuitResult.h
      TGo4FitModelFormula.h
      TGo4FitModelFromData.h
      TGo4FitModelFunction.h
      TGo4FitModelGauss1.h
      TGo4FitModelGauss2.h
      TGo4FitModelGaussN.h
      TGo4FitModel.h
      TGo4FitModelPolynom.h
      TGo4FitNamed.h
      TGo4FitParameter.h
      TGo4FitParsList.h
      TGo4FitPeakFinder.h
      TGo4FitSlot.h
      TGo4FitterAbstract.h
      TGo4FitterAction.h
      TGo4FitterConfig.h
      TGo4Fitter.h
      TGo4FitterOutput.h
)

set(Go4Fit_sources
      TGo4FitAmplEstimation.cxx
      TGo4FitAxisTrans.cxx
      TGo4FitComponent.cxx
      TGo4FitData.cxx
      TGo4FitDataGraph.cxx
      TGo4FitDataHistogram.cxx
      TGo4FitDataProfile.cxx
      TGo4FitDataRidge.cxx
      TGo4FitDependency.cxx
      TGo4FitLinearTrans.cxx
      TGo4FitMatrixTrans.cxx
      TGo4FitMinuit.cxx
      TGo4FitMinuitResult.cxx
      TGo4FitModel.cxx
      TGo4FitModelFormula.cxx
      TGo4FitModelFromData.cxx
      TGo4FitModelFunction.cxx
      TGo4FitModelGauss1.cxx
      TGo4FitModelGauss2.cxx
      TGo4FitModelGaussN.cxx
      TGo4FitModelPolynom.cxx
      TGo4FitNamed.cxx
      TGo4FitParameter.cxx
      TGo4FitParsList.cxx
      TGo4FitPeakFinder.cxx
      TGo4FitSlot.cxx
      TGo4FitterAbstract.cxx
      TGo4FitterAction.cxx
      TGo4FitterConfig.cxx
      TGo4Fitter.cxx
      TGo4FitterOutput.cxx
)

GO4_INSTALL_HEADERS(${Go4Fit_headers})

GO4_STANDARD_LIBRARY(Go4Fit
                     LINKDEF Go4FitLinkDef.h
                     HEADERS ${Go4Fit_headers}
                     SOURCES ${Go4Fit_sources}
                     LIBRARIES Minuit Spectrum)
