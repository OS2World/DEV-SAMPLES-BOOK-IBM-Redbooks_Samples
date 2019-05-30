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
***       OS/2 Version 2.0 - Volume 4: Writing Applications         ***
***                                                                 ***
***                           GG24-3774-01                          ***
***                                                                 ***
***      March 1st 1993 version - includes PWFolder and PWFin       ***
***                                                                 ***
***********************************************************************

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


Notes on WPS Redbook Examples
=============================

Prerequisite levels of OS/2

These samples can be run on OS/2 2.0 with Service Pak XR06055 or later. If you 
try to run them on OS/2 2.0 GA, you may encounter problems. Please apply the 
Service Pak XR06055, or at least the SOMFIX package available on OS2TOOLS or
Compuserve (this contains a replacement SOM.DLL for OS/2 2.0 GA code)

General

The WPS samples each come with the following REXX programs:

 - ADD.CMD
   This installs the example classes and should only be done once
   for each example, (Re-boot may be required to perform the
   ADD.CMD for a second example).

   This program prompts for the drive letter of the boot drive, this is needed 
   in order to copy the DLL to the correct subdirectory

 - TEST.CMD
   This creates the objects

 - REMOVE.CMD
   This Deregisters the classes. Re-boot is required after
   this step, then the DLLs that ADD.CMD copied in to the
   \OS2\DLL directory may be deleted.

 - BUILD.CMD
   This runs the NMAKE utility for the samples.
   The way the make files are set up they expect the
   toolkit to be installed on the C drive, and the
   CSET environment to be set.

1.PWFolder class example

Files:  PWFOLDER.DLL  - A WPFolder derived class that can be locked
                        with a user specified password

The PWFolder password is passed in via a Setup string. For the
example TEST.CMD sets the password to "wps" by specifying
"PASSWORD=wps;" setup string in the SysCreateObject command.

2.PWFinanceFIle class example

Files:  PWFIN.DLL  - A WPDatFile derived class that can be locked
                     with a user specified password.  This example is
                     similar to the PWFolder example, but includes some  
                     Drag and Drop code, as well as a skeleton for a custom
                     view and other enhancements.
                     This code was written by Douglas Pearless of IBM New Zealand Ltd
                     to illustrate Workplace programming concepts, especially those
                     covered by his updates to Volume 4.

The PWFinanceFile password is passed in via a Setup string. For the
example TEST.CMD sets the password to "wps" by specifying
"PASSWORD=wps;" setup string in the SysCreateObject command.

Known bug:

When selecting the context menu, then open and then the System Editor (The
default for this derived class), the task list appears!!! - Alternatively
double clicking with the left hand mouse button is the same as the 
open described!              
Interestingly enough if I open the system editor settings and remove all the
associations, and then for the enhanced editor I add the text association so
that the enhanced editor now replaces the system editor it works correctly.  
Unfortunately this bug became apparent at the end of the time available to 
create this same code.  Consequently the author of this code did not have 
sufficient time to solve this problem before publication.  It could therefore
be left as an exercise for the reader!

*************************************************************************************



The other Redbooks, covering OS/2 Version 2.0 are:


GG24-3730  "OS/2 Version 2.0 - Volume 1: Control Program"

(Sample code available in package RB3730.ZIP on CompuServe
 and GG243730 PACKAGE on OS2TOOLS)

GG24-3731  "OS/2 Version 2.0 - Volume 2: DOS and Windows Environment"

(Sample code available in package RB3731.ZIP on CompuServe
 and GG243731 PACKAGE on OS2TOOLS)

GG24-3732  "OS/2 Version 2.0 - Volume 3: Presentation Manager & Workplace Shell"

GG24-3775  "OS/2 Version 2.0 - Volume 5: Print Subsystem"

------------------------------------------------------------------------

