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
***       OS/2 Version 2.0 - Volume 4: Application Development      ***
***                                                                 ***
***                           GG24-3774                             ***
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


Notes on WPS Redbook Examples
=============================

General

The WPS samples each come with the following REXX programs:

 - ADD.CMD
   This installs the example classes and should only be done once
   for each example, (Re-boot is required to perform the
   ADD.CMD for a second example).

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


2.Record Class example

Files:  RECORD.DLL    - Transient record class
        RECFLDER.DLL  - WPFolder derived class that has
                        a context menu option to do a telephone
                        list find.
        SERVER.EXE    - Sever program (pretends to access a database
                        but actually reads the names and address from
                        an ASCII file; c:\names.dat).



The Record class has been broken down in to separate threads
and processes as follows:

     WPS        3      WPS Process   33   Second Process
     Thread            New Thread
                3                    33

                3      Client        33     Server
   ZDDDDDDDDD?       ZDDDDDDDDDDD?        ZDDDDDDDDDDD?
   3 Record  3  3    3 Database  3   33   3 Database  3
   3 Class   3       3 requester 3        3 server    3
   3         3  3    3 PM object 3   33   3 PM object 3
   @DDDDDDDDDY       3 window    3        3 window    3
                3    @DDDDDDDDDDDY   33   @DDDDDDDDDDDY

                3                    33   ZDDDDDDDD?
                                          3 ZDDDDDDDD?    Perform
                3                    33   3 3 ZDDDDDDDD?  actual
                                          @D3 3 Work   3  searches
                3                    33     @D3 Thread 3
                                              @DDDDDDDDY


The Record class receives messages to search its database
and instantiate new records from a folder object.

                  Folder object                Record Class
                 ZDDDDDDDDDDDDDDDDDDD?        ZDDDDDDDDDDDDDDDDDDD?
                 3                   3        3                   3
  ZDDDD?         CDDDDDDDDDDDDDDDDDDD4        CDDDDDDDDDDDDDDDDDDD4
  3User3 DDDDDDD>3Search for records 3DDDDDDD>3clsQueryDatabase   3
  @DDDDY         CDDDDDDDDDDDDDDDDDDD4        CDDDDDDDDDDDDDDDDDDD4
                 3                   3        3                   3
                 @DDDDDDDDDDDDDDDDDDDY        @DDDDDDDDDDDDDDDDDDDY
To enable the object communication between the Folder object and
the Record class we do the following:

In the folder class .C

        #include "record.h"

In the folder class .DEF

        IMPORTS
           record.RecordCClassData
           record.RecordClassData
           record.RecordNewClass
           record.M_RecordCClassData
           record.M_RecordClassData
           record.M_RecordNewClass

This allows us to directly call the Record class method:

      _clsQueryDatabase(RecordClass, pQuery, Folder);




The record class processes the _clsQueryDatabase method by
copying all the necessary information to a newly allocated
block of shared memory and sending it to the Requester PM
object window.

   Record Class object
   ZDDDDDDDDDDDDDDDDDDDDD?     ZDDDDDDDD?
   3 _clsQueryDatabase() 3     3Shared  3     Requester Object window
   3                     3     3memory  3     ZDDDDDDDDDDDDDDDDDDD?
   3  DosAllocShrMem     3     @DDDDDDDDY     3        .          3
   3  <Fill memory>      3     WinSendMsg     3        .          3
   3  WinSendMsg         3 DDDDDDDDDDDDDDDDDD>3 case WMP_DO_FIND: 3
   3                     3                    3        .          3
   @DDDDDDDDDDDDDDDDDDDDDY                    3        .          3
                                              @DDDDDDDDDDDDDDDDDDDY

The Requester object window passes the message on
to the Server object window and then frees it from its
own process. A successful transfer has been made the
memory is now "owned" soley by the Server process.

  Requester Object window
  ZDDDDDDDDDDDDDDDDDDD?       ZDDDDDDDD?
  3         .         3       3Shared  3     Server Object window
  3         .         3       3memory  3     ZDDDDDDDDDDDDDDDDDDD?
  3 case WMP_DO_FIND: 3       @DDDDDDDDY     3        .          3
  3  DosGiveShrMem    3     WinSendMsg       3        .          3
  3  WinSendMsg       3DDDDDDDDDDDDDDDDDDDDD>3 case WMP_DO_FIND: 3
  3  DosFreeMem       3                      3        .          3
  @DDDDDDDDDDDDDDDDDDDY                      3        .          3
                                             @DDDDDDDDDDDDDDDDDDDY

The server now looks at the content message, specifically
if the query is null it means that the folder object did
send any query data, and a dialog should be created to ask the
user for some query data. It is worth noting that to bring up
the dialog box the server object window sends a message to
the main server application window; this is due to the fact
that UI windows cannot be created in HWND_OBJECT threads.

Once a valid query is ready a work thread is started to
search the database. The results found are sent back to
server object window and the thread terminates.


      ZDDDDDDDDDD?  Start Thread    ZDDDDDDDD?
      3 Database 3D D D D D D D D D>3 Work   3 Perform search
      3 server   3    DO_FIND       3 Thread 3 DosAllocShrMem [results]
      3 object   3                  @DDDDDDDDY DosFreeMem [query data]
      3 window   3<DDDDDDD?              3
      @DDDDDDDDDDY        3              3
                          @DDDDDDDDDDDDDDY
                             RESULTS





On receiving the results the server object window in turn
passes them back to the Requester object window. Using the
results data the Requester object window creates new
record objects inside the folder that requested the
search in the first place.

      ZDDDDDDDDDD?
      3 Database 3 DosGiveShrMem [results]
      3 server   3
      3 object   3 DosFreeMem [results]
      3 window   3 (after sending)
      @DDDDDDDDDDY
           3
           3                          ZDBDDDDDDDDDDDDDDDDDDDDDDDDDDBDBD?
           3                          CDADDDDDDDDDDDDDDDDDDDDDDDDDDADAD4
           3RESULTS                   3                                3
           3                          3          ZDDDD?                3
           V                          3          3    3     ZDDDD?     3
      ZDDDDDDDDDDD?                   3          @DDDDY     3    3     3
      3 Database  3    _wpclsNew      3      ZDDDD?         @DDDDY     3
      3 requester 3  DDDDDDDDDDDDDDDDDDDDDDD>3    3                    3
      3 object    3                   3      @DDDDY                    3
      3 window    3                   3                                3
      @DDDDDDDDDDDY                   @DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDY
       DosFreeMem [results]


Notes on DROPINFO Example
=========================

The DROPINFO example displays the contents of the DRAGINFO and DRAGITEM data
structures whenever another object is dropped over the DROPINFO program's
window.  This example is intended to show the sequence of messages which
occurs during a drop operation, and to enable testing/debugging of drag/drop
applications.

The DROPINFO example comes with a command file named BUILD.CMD, which invokes
the NMAKE utility.


**************************************************************************************

DMCUST.C - a demo program by Alan Chambers - UK PS Technical Support, whilst on
           a residency at the ITSC, Boca Raton

Updated to work with GA code, 6 May 1992

This program reads several names, addresses and phone numbers from a file (DMCUST.DAT)
and displays them as icons in a container window.  These items can be moved from
one instance of this program to another by direct manipulation; they can be dropped
on an order form displayed by the DMORDER sample to fill in the customer details therein,
and they can be dropped on the telephone icon shown by the DMPHONE sample, to simulate
a telephone dialer.


This sample illustrates several different aspects of drag/drop:

    - initiating a drag from a container window
    - using the DRM_PRINT rendering mechanism to allow application printing by
      drag/drop to a Workplace Shell printer object
    - using the DRM_DISCARD rendering mechanism to allow deletion of application
      items by dragging them to the Workplace Shell shredder object
    - implementing both ends of a user defined rendering mechanism (here called
      DRM_SHAREMEM)


The program concentrates on demonstrating drag/drop techniques and is therefore
very simple in other respects. In particular, the so called customers to be displayed
are read from a simple flat file called DMCUST.DAT that can be read with fscanf().
the following lines illustrate the format of this file - note that each text
field must have no imbedded blanks:

It does also support
printing by drag/drop to a WPS printer object, and deletion by drag/drop to
the shredder.  This illustrates the use of the DRM_PRINT and DRM_DISCARD
rendering mechanism.

Please note that I am demonstrating the use of the DRM_PRINT drag/drop
technique, NOT PM printing.  To keep the sample simple, the actual printing is
done by opening LPT1 and writing data to it - the result is that whichever
printer to drag to, the printing comes out on LPT1.  To put the proper PM
printing in would have distracted attention from the real point of the sample -
ie direct manipulation.

**************************************************************************************

DMORDER.C - a demo program written by Franco Federico and Alan Chambers
            whilst on a residency at the ITSC, Boca Raton

Updated to work with GA code, 6 May 1992

This program will display an order form when the appropriate action bar selection
is made.  Customers, dragged from the DMCUST sample program, can be dropped onto
this order form, with the result that the customer's details are entered into
the appropriate fields.

The code for the window and dialog box, and the technique used for subclassing
were written by Franco, the drag/drop code added by Alan.


**************************************************************************************

DMPHONE.C - a demo program by Alan Chambers - UK PS Technical Support, on residency
           at the ITSC, Boca Raton

Modified 6 May 1992 to work with GA code.


This program is designed to be used with the DMCUST sample program.  A customer
can be dragged onto the telephone icon displayed by this program, with the result
that a series of beeps, reminiscent of touchtone dialling tones, are sounded.

The program illustrates the use of a private rendering mechanism, DRM_SHAREMEM, from
the target's point of view.

The program displays a telephone icon as using the WC_STATIC class, then subclasses
this static window to add function for drag/drop, moving the icon around the desktop,
and providing a context (pop-up) menu.

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
