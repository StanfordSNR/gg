$! Test_make.com
$!
$! This is a wrapper for the GNU make perl test programs on VMS.
$!
$! Parameter "-help" for description on how to use described below.
$!
$! Copyright (C) 2014-2016 Free Software Foundation, Inc.
$! This file is part of GNU Make.
$!
$! GNU Make is free software; you can redistribute it and/or modify it under
$! the terms of the GNU General Public License as published by the Free Software
$! Foundation; either version 3 of the License, or (at your option) any later
$! version.
$!
$! GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
$! WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
$! FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
$! details.
$!
$! You should have received a copy of the GNU General Public License along with
$! this program.  If not, see <http://www.gnu.org/licenses/>.
$!
$!
$! Allow more than 8 paramters with using commas as a delimiter.
$!
$ params = "''p1',''p2',''p3',''p4',''p5',''p6',''p7',''p8'"
$!
$ test_flags = ",verbose,detail,keep,usage,help,debug,"
$ test_flags_len = f$length(test_flags)
$ verbose_flag = ""
$ detail_flag = ""
$ keep_flag = ""
$ usage_flag = ""
$ help_flag = ""
$ debug_flag = ""
$!
$ ignored_options = "profile,make,srcdir,valgrind,memcheck,massif,"
$ ignored_option_len = f$length(ignored_options)
$!
$ testname = ""
$ make :== $bin:make.exe"
$!
$ i = 0
$param_loop:
$ param = f$element(i, ",", params)
$ i = i + 1
$ if param .eqs. "" then goto param_loop
$ if param .eqs. "," then goto param_loop_end
$ param_len = f$length(param)
$ if f$locate("/", param) .lt. param_len
$ then
$   if testname .nes. ""
$   then
$       write sys$output "Only the last test name specified will be run!"
$   endif
$   testname = param
$   goto param_loop
$ endif
$ lc_param = f$edit(param,"LOWERCASE") - "-"
$ if f$locate(",''lc_param',", ignored_options) .lt. ignored_option_len
$ then
$   write sys$output "parameter ''param' is ignored on VMS for now."
$   goto param_loop
$ endif
$ if f$locate(",''lc_param',", test_flags) .lt. test_flags_len
$ then
$   'lc_param'_flag = "-" + lc_param
$   goto param_loop
$ endif
$   write sys$output "parameter ''param' is not known to VMS."
$ goto param_loop
$!
$param_loop_end:
$!
$no_gnv = 1
$no_perl = 1
$!
$!  Find GNV 2.1.3 + manditory updates
$!  If properly updated, the GNV$GNU logical name is present.
$!  Updated GNV utilities have a gnv$ prefix on them.
$   gnv_root = f$trnlnm("GNV$GNU", "LNM$SYSTEM_TABLE")
$   if gnv_root .nes. ""
$   then
$       no_gnv = 0
$       ! Check for update ar utility.
$       new_ar = "gnv$gnu:[usr.bin]gnv$ar.exe"
$       if f$search(new_ar) .nes. ""
$       then
$           ! See if a new port of ar exists.
$           ar :== $'new_ar'
$       else
$           ! Fall back to legacy GNV AR wrapper.
$           old_ar = "gnv$gnu:[bin]ar.exe"
$           if f$search(old_ar) .nes. ""
$           then
$               ar :== $'old_ar'
$           else
$               no_gnv = 1
$           endif
$       endif
$       ! Check for updated bash
$       if no_gnv .eq. 0
$       then
$           new_bash = "gnv$gnu:[bin]gnv$bash.exe"
$           if f$search(new_bash) .nes. ""
$           then
$               bash :== $'new_bash'
$               sh :== $'new_bash'
$           else
$               no_gnv = 1
$           endif
$       endif
$       ! Check for updated coreutils
$       if no_gnv .eq. 0
$       then
$           new_cat = "gnv$gnu:[bin]gnv$cat.exe"
$           if f$search(new_cat) .nes. ""
$           then
$               cat :== $'new_cat'
$               cp :== $gnv$gnu:[bin]gnv$cp.exe
$               echo :== $gnv$gnu:[bin]gnv$echo.exe
$               false :== $gnv$gnu:[bin]gnv$false.exe
$               true :== $gnv$gnu:[bin]gnv$true.exe
$               touch :== $gnv$gnu:[bin]gnv$touch.exe
$               mkdir :== $gnv$gnu:[bin]gnv$mkdir.exe
$               rm :== $gnv$gnu:[bin]gnv$rm.exe
$               sleep :== $gnv$gnu:[bin]gnv$sleep.exe
$           else
$               no_gnv = 1
$           endif
$       endif
$       ! Check for updated diff utility.
$       if no_gnv .eq. 0
$       then
$           new_diff = "gnv$gnu:[usr.bin]gnv$diff.exe"
$           if f$search(new_diff) .nes. ""
$           then
$               ! See if a new port of diff exists.
$               diff :== $'new_diff'
$           else
$               ! Fall back to legacy GNV diff
$               old_diff = "gnv$gnu:[bin]diff.exe"
$               if f$search(old_diff) .nes. ""
$               then
$                   diff :== $'old_diff'
$               else
$                   no_gnv = 1
$               endif
$           endif
$       endif
$   endif
$!
$if no_gnv
$then
$   write sys$output "Could not find an up to date GNV installed!"
$   help_flag = 1
$endif
$!
$! Find perl 5.18.1 or later.
$!
$! look in perl_root:[000000]perl_setup.com
$ perl_root = f$trnlnm("perl_root")
$ ! This works with known perl installed from PCSI kits.
$ if perl_root .nes. ""
$ then
$   perl_ver = f$element(1, ".", perl_root)
$   if f$locate("-", perl_ver) .lt. f$length(perl_ver)
$   then
$       no_perl = 0
$   endif
$ endif
$ if no_perl
$ then
$!  look for sys$common:[perl-*]perl_setup.com
$   perl_setup = f$search("sys$common:[perl-*]perl_setup.com")
$   if perl_setup .eqs. ""
$   then
$       if gnv_root .nes. ""
$       then
$           gnv_device = f$parse(gnv_root,,,"DEVICE")
$           perl_templ = "[vms$common.perl-*]perl_setup.com"
$           perl_search = f$parse(perl_templ, gnv_device)
$           perl_setup = f$search(perl_search)
$       endif
$   endif
$   if perl_setup .nes. ""
$   then
$       @'perl_setup'
$       no_perl = 0
$   endif
$ endif
$!
$ if no_perl
$ then
$   write sys$output "Could not find an up to date Perl installed!"
$   help_flag = "-help"
$ endif
$!
$!
$ if help_flag .nes. ""
$ then
$   type sys$input
$DECK
This is a test script wrapper for the run_make_tests.pl script.

This wrapper makes sure that the DCL symbols and logical names needed to
run the perl script are in place.

The test wrapper currently requires that the DCL symbols be global symbols.
Those symbols will be left behind after the procedure is run.

The PERL_ROOT will be set to a compatible perl if such a perl is found and
is not the default PERL_ROOT:.  This setting will persist after the test.

This wrapper should be run with the default set to the base directory
of the make source.

The HELP parameter will bring up this text and then run the help script
for the Perl wrapper.  Not all options for the perl script have been
implemented, such as valgrind or specifying the make path or source path.

Running the wrapper script requires:
  Perl 5.18 or later.
  PCSI kits available from http://sourceforge.net/projects/vmsperlkit/files/

  GNV 2.1.3 or later.  GNV 3.0.1 has not tested with this script.
  Bash 4.2.47 or later.
  Coreutils 8.21 or later.
  http://sourceforge.net/projects/gnv/files/
  Read before installing:
     http://sourceforge.net/p/gnv/wiki/InstallingGNVPackages/
  As updates for other GNV components get posted, those updates should
  be used.

$EOD
$ endif
$!
$ if no_gnv .or. no_perl then exit 44
$!
$!
$ default = f$environment("DEFAULT")
$ default_dev = f$element(0, ":", default) + ":"
$ this = f$environment("PROCEDURE")
$ on error then goto all_error
$ set default 'default_dev''f$parse(this,,,"DIRECTORY")'
$!
$! Need to make sure that the config-flags.pm exists.
$ if f$search("config-flags.pm") .eqs. ""
$ then
$   @config_flags_pm.com
$ endif
$ define/user bin 'default_dev'[-],gnv$gnu:[bin]
$ define/user decc$filename_unix_noversion enable
$ define/user decc$filename_unix_report enable
$ define/user decc$readdir_dropdotnotype enable
$ flags = ""
$ if verbose_flag .nes. "" then flags = verbose_flag
$ if detail_flag .nes. "" then flags = flags + " " + detail_flag
$ if keep_flag .nes. "" then flags = flags + " " + keep_flag
$ if usage_flag .nes. "" then flags = flags + " " + usage_flag
$ if help_flag .nes. "" then flags = flags + " " + help_flag
$ if debug_flag .nes. "" then flags = flags + " " + debug_flag
$ flags = f$edit(flags, "TRIM, COMPRESS")
$ if testname .nes. ""
$ then
$   perl run_make_tests.pl "''testname'" 'flags'
$ else
$   perl run_make_tests.pl 'flags'
$ endif
$all_error:
$ set default 'default'
$!
