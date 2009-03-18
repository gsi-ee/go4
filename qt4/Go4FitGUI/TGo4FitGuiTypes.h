#ifndef TGO4FITGUITYPES_H
#define TGO4FITGUITYPES_H

namespace FitGui {

  enum ItemType
    { ot_none = -1, ot_empty,
      ot_datalist, ot_data, ot_modellist, ot_model, ot_parslist, ot_par, ot_parcfg,
      ot_asslist, ot_ass, ot_rangelist, ot_range, ot_rangecut,
      ot_actlist, ot_action, ot_amplest, ot_peakfinder, ot_output, ot_config, ot_minuit,
      ot_parinitlist, ot_pardeplist, ot_reslist,
      ot_depend, ot_newpars, ot_parscfg, ot_minuitres, ot_slot, ot_fitter,
      ot_trans, ot_lintrans, ot_matrtrans,
      ot_results, ot_allslots,
      ot_wizdatalist };

  enum MenuType
     { mt_none = -1, mt_empty,
       mt_datalist, mt_data, mt_modellist, mt_model, mt_parslist, mt_par, mt_parcfg,
       mt_asslist, mt_ass, mt_rangelist, mt_range, mt_rangecut,
       mt_actlist, mt_action,
       mt_deplist,
       mt_depend, mt_newpars,  mt_minuitres, mt_slot, mt_fitter,
       mt_allslots  };

   enum WidgetType
     { wt_none = -1, wt_par, wt_data, wt_model, wt_polynom, wt_gauss1, wt_gauss2, wt_gaussn, wt_formula,
       wt_function, wt_fromdata, wt_range, wt_rangecut, wt_amplest, wt_peakfinder, wt_minuit, wt_output, wt_depend, wt_parcfg,
       wt_minuitres, wt_slot, wt_fitter, wt_named, wt_lintrans, wt_matrtrans, wt_table };

   enum GraphType
     { gt_none = -1, gt_data, gt_model, gt_range, gt_ranges, gt_ass, gt_fitter };

   enum PanelMode
     { pm_None = 0, pm_Simple = 1, pm_Wizard = 2, pm_Expert = 3};

   enum { ListStackId = 111 };
};

#endif
