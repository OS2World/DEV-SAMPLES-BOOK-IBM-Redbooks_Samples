include <wptrans.sc>

class: GEQueryDesktop,
       file stem = qdesk,
       external stem = qt,
       external prefix = qt_,
       class prefix = qtM_,
       major version = 1,
       minor version = 1,
       local;

parent: WPTransient;

data:

  PVOID pDummy;

methods:

  override wpSetup;
  override wpclsQueryStyle, class;

