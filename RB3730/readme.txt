***********************************************************************
***                                                                 ***
*** IBM International Technical Support Center, Boca Raton, Florida ***
***                                                                 ***
***              OS/2 Version 2.0 Technical Compendium              ***
***                                                                 ***
***                           GBOF-2254                             ***
***                                                                 ***
***                        Sample Code for                          ***
***                                                                 ***
***           OS/2 Version 2.0 - Volume 1: Control Program          ***
***                                                                 ***
***                           GG24-3730                             ***
***                                                                 ***
***********************************************************************


Please use the ZOO 2.1 shareware utility to unpack the files.

*************************** Disclaimer ********************************

References in this package to IBM products, programs or services do not
imply that IBM intends to make these available in all countries in which
IBM operates.  Any reference to an IBM product, program, or service is not
intended to state or imply that only IBM's product, program, or service may
be used.  Any functionally equivalent program that does not infringe any of
IBM's intellectual property rights may be used instead of the IBM product,
program or service.

Information in this package was developed in conjunction with use of the
equipment specified, and is limited in application to those specific
hardware and software products and levels.

IBM may have patents or pending patent applications covering subject matter
in this package.  The furnishing of this package does not give you any
license to these patents.  You can send license inquiries, in writing, to
the IBM Director of Commercial Relations, IBM Corporation, Purchase,
NY 10577.

The information contained in this package has not been submitted to any
formal IBM test and is distributed AS IS.

The use of this information or the implementation of any of these
techniques is a customer responsibility and depends on the customer's
ability to evaluate and integrate them into the customer's operational
environment.  While each item may have been reviewed by IBM for accuracy in
a specific situation, there is no guarantee that the same or similar
results will be obtained elsewhere.  Customers attempting to adapt these
techniques to their own environments do so at their own risk.

This package contains examples of data used in daily
business operations.  To illustrate them as completely as possible, the
examples contain the names of individuals, companies, brands, and products.
All of these names are fictitious and any similarity to the names and
addresses used by an actual business enterprise is entirely coincidental.

*********************** End of Disclaimer *****************************


Notes on Control Program Redbook Examples
=========================================

MEMLAB1:

  Demonstrates the use of the new
  DosAllocMem API, and the handling
  of General Protection Exceptions.

MEMLAB2:

  Demonstrates the different types
  of memory allocation.

MEMLAB3:

  Demonstrates multiple DOS sessions
  started from a OS/2 program, and
  to show how the size of the swap
  file varies as the sessions are
  started and what happens to the swap
  file after the sessions are stopped.

MEMLAB4:

  Demonstrates the new system
  limit for the number of threads
  per process in OS/2 2.0 and the
  effect of thread creation on the
  growth of SWAPPER.DAT.

SWAPSIZE:

  To be used with the Lab Session
  Examples given in Appendix C of the
  OS/2 Version 2.0 Volume 1: Control
  Program Document no GG24-3730.
  This program interrogates and
  displays the size of the
  SWAPPER.DAT at regular intervals
  in a PM window. The interval is
  initially set to 10 seconds. This
  may be changed to 30 seconds or
  60 seconds by selecting the
  Interval action bar.




The other Redbooks, covering OS/2 Version 2.0 are:


GG24-3731  "OS/2 Version 2.0 - Volume 2: DOS and Windows Environment"

(Sample code available in package RB3731.ZIP on CompuServe
 and GG243731 PACKAGE on OS2TOOLS)

GG24-3732  "OS/2 Version 2.0 - Volume 3: Presentation Manager & Workplace Shell"

GG24-3774  "OS/2 Version 2.0 - Volume 4: Application Development"

(Sample code available in package RB3774.ZIP on CompuServe
 and GG243774 PACKAGE on OS2TOOLS)

GG24-3775  "OS/2 Version 2.0 - Volume 5: Print Subsystem"

(Sample code available in package RB3775.ZIP on CompuServe
 and GG243775 PACKAGE on OS2TOOLS)

------------------------------------------------------------------------

