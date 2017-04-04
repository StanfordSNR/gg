#!/usr/bin/env perl
# -*-perl-*-

# Test driver for the Make test suite

# Usage:  run_make_tests  [testname]
#                         [-debug]
#                         [-help]
#                         [-verbose]
#                         [-keep]
#                         [-make <make prog>]
#                        (and others)

# Copyright (C) 1992-2016 Free Software Foundation, Inc.
# This file is part of GNU Make.
#
# GNU Make is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.
#
# GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

%FEATURES = ();

$valgrind = 0;              # invoke make with valgrind
$valgrind_args = '';
$memcheck_args = '--num-callers=15 --tool=memcheck --leak-check=full --suppressions=guile.supp';
$massif_args = '--num-callers=15 --tool=massif --alloc-fn=xmalloc --alloc-fn=xcalloc --alloc-fn=xrealloc --alloc-fn=xstrdup --alloc-fn=xstrndup';
$pure_log = undef;

# The location of the GNU make source directory
$srcdir = '';

$command_string = '';

$all_tests = 0;

# rmdir broken in some Perls on VMS.
if ($^O eq 'VMS')
{
  require VMS::Filespec;
  VMS::Filespec->import();

  sub vms_rmdir {
    my $vms_file = vmspath($_[0]);
    $vms_file = fileify($vms_file);
    my $ret = unlink(vmsify($vms_file));
    return $ret
  };

  *CORE::GLOBAL::rmdir = \&vms_rmdir;
}

require "test_driver.pl";
require "config-flags.pm";

# Some target systems might not have the POSIX module...
$has_POSIX = eval { require "POSIX.pm" };

#$SIG{INT} = sub { print STDERR "Caught a signal!\n"; die @_; };

sub valid_option
{
   local($option) = @_;

   if ($option =~ /^-make([-_]?path)?$/i) {
       $make_path = shift @argv;
       if (!-f $make_path) {
           print "$option $make_path: Not found.\n";
           exit 0;
       }
       return 1;
   }

   if ($option =~ /^-srcdir$/i) {
       $srcdir = shift @argv;
       if (! -f "$srcdir/gnumake.h") {
           print "$option $srcdir: Not a valid GNU make source directory.\n";
           exit 0;
       }
       return 1;
   }

   if ($option =~ /^-all([-_]?tests)?$/i) {
       $all_tests = 1;
       return 1;
   }

   if ($option =~ /^-(valgrind|memcheck)$/i) {
       $valgrind = 1;
       $valgrind_args = $memcheck_args;
       return 1;
   }

   if ($option =~ /^-massif$/i) {
       $valgrind = 1;
       $valgrind_args = $massif_args;
       return 1;
   }

# This doesn't work--it _should_!  Someone badly needs to fix this.
#
#   elsif ($option =~ /^-work([-_]?dir)?$/)
#   {
#      $workdir = shift @argv;
#      return 1;
#   }

   return 0;
}


# This is an "all-in-one" function.  Arguments are as follows:
#
#  [0] (string):  The makefile to be tested.  undef means use the last one.
#  [1] (string):  Arguments to pass to make.
#  [2] (string):  Answer we should get back.
#  [3] (integer): Exit code we expect.  A missing code means 0 (success)

$old_makefile = undef;

sub subst_make_string
{
    local $_ = shift;
    $makefile and s/#MAKEFILE#/$makefile/g;
    s/#MAKEPATH#/$mkpath/g;
    s/#MAKE#/$make_name/g;
    s/#PERL#/$perl_name/g;
    s/#PWD#/$pwd/g;
    return $_;
}

sub run_make_test
{
  local ($makestring, $options, $answer, $err_code, $timeout) = @_;
  my @call = caller;

  # If the user specified a makefile string, create a new makefile to contain
  # it.  If the first value is not defined, use the last one (if there is
  # one).

  if (! defined $makestring) {
    defined $old_makefile
      || die "run_make_test(undef) invoked before run_make_test('...')\n";
    $makefile = $old_makefile;
  } else {
    if (! defined($makefile)) {
      $makefile = &get_tmpfile();
    }

    # Make sure it ends in a newline and substitute any special tokens.
    $makestring && $makestring !~ /\n$/s and $makestring .= "\n";
    $makestring = subst_make_string($makestring);

    # Populate the makefile!
    open(MAKEFILE, "> $makefile") || die "Failed to open $makefile: $!\n";
    print MAKEFILE $makestring;
    close(MAKEFILE) || die "Failed to write $makefile: $!\n";
  }

  # Do the same processing on $answer as we did on $makestring.
  if (defined $answer) {
      $answer && $answer !~ /\n$/s and $answer .= "\n";
      $answer = subst_make_string($answer);
  }

  run_make_with_options($makefile, $options, &get_logfile(0),
                        $err_code, $timeout, @call);
  &compare_output($answer, &get_logfile(1));

  $old_makefile = $makefile;
  $makefile = undef;
}

# The old-fashioned way...
sub run_make_with_options {
  my ($filename,$options,$logname,$expected_code,$timeout,@call) = @_;
  @call = caller unless @call;
  local($code);
  local($command) = $make_path;

  $expected_code = 0 unless defined($expected_code);

  # Reset to reflect this one test.
  $test_passed = 1;

  if ($filename) {
    $command .= " -f $filename";
  }

  if ($options) {
    if ($^O eq 'VMS') {
      # Try to make sure arguments are properly quoted.
      # This does not handle all cases.

      # VMS uses double quotes instead of single quotes.
      $options =~ s/\'/\"/g;

      # If the leading quote is inside non-whitespace, then the
      # quote must be doubled, because it will be enclosed in another
      # set of quotes.
      $options =~ s/(\S)(\".*\")/$1\"$2\"/g;

      # Options must be quoted to preserve case if not already quoted.
      $options =~ s/(\S+)/\"$1\"/g;

      # Special fixup for embedded quotes.
      $options =~ s/(\"\".+)\"(\s+)\"(.+\"\")/$1$2$3/g;

      $options =~ s/(\A)(?:\"\")(.+)(?:\"\")/$1\"$2\"/g;

      # Special fixup for misc/general4 test.
      $options =~ s/""\@echo" "cc""/\@echo cc"/;
      $options =~ s/"\@echo link"""/\@echo link"/;

      # Remove shell escapes expected to be removed by bash
      if ($options !~ /path=pre/) {
        $options =~ s/\\//g;
      }

      # special fixup for options/eval
      $options =~ s/"--eval=\$\(info" "eval/"--eval=\$\(info eval/;

      print ("Options fixup = -$options-\n") if $debug;
    }
    $command .= " $options";
  }

  $command_string = "";
  if (@call) {
      $command_string = "#$call[1]:$call[2]\n";
  }
  $command_string .= "$command\n";

  if ($valgrind) {
    print VALGRIND "\n\nExecuting: $command\n";
  }


  {
      my $old_timeout = $test_timeout;
      $timeout and $test_timeout = $timeout;

      # If valgrind is enabled, turn off the timeout check
      $valgrind and $test_timeout = 0;

      $code = &run_command_with_output($logname,$command);
      $test_timeout = $old_timeout;
  }

  # Check to see if we have Purify errors.  If so, keep the logfile.
  # For this to work you need to build with the Purify flag -exit-status=yes

  if ($pure_log && -f $pure_log) {
    if ($code & 0x7000) {
      $code &= ~0x7000;

      # If we have a purify log, save it
      $tn = $pure_testname . ($num_of_logfiles ? ".$num_of_logfiles" : "");
      print("Renaming purify log file to $tn\n") if $debug;
      rename($pure_log, "$tn")
        || die "Can't rename $log to $tn: $!\n";
      ++$purify_errors;
    } else {
      unlink($pure_log);
    }
  }

  if ($code != $expected_code) {
    print "Error running $make_path (expected $expected_code; got $code): $command\n";
    $test_passed = 0;
    $runf = &get_runfile;
    &create_file (&get_runfile, $command_string);
    # If it's a SIGINT, stop here
    if ($code & 127) {
      print STDERR "\nCaught signal ".($code & 127)."!\n";
      ($code & 127) == 2 and exit($code);
    }
    return 0;
  }

  if ($profile & $vos) {
    system "add_profile $make_path";
  }

  return 1;
}

sub print_usage
{
   &print_standard_usage ("run_make_tests",
                          "[-make MAKE_PATHNAME] [-srcdir SRCDIR] [-memcheck] [-massif]",);
}

sub print_help
{
   &print_standard_help (
        "-make",
        "\tYou may specify the pathname of the copy of make to run.",
        "-srcdir",
        "\tSpecify the make source directory.",
        "-valgrind",
        "-memcheck",
        "\tRun the test suite under valgrind's memcheck tool.",
        "\tChange the default valgrind args with the VALGRIND_ARGS env var.",
        "-massif",
        "\tRun the test suite under valgrind's massif toool.",
        "\tChange the default valgrind args with the VALGRIND_ARGS env var."
       );
}

sub get_this_pwd {
  $delete_command = 'rm -f';
  if ($has_POSIX) {
    $__pwd = POSIX::getcwd();
  } elsif ($vos) {
    $delete_command = "delete_file -no_ask";
    $__pwd = `++(current_dir)`;
  } else {
    # No idea... just try using pwd as a last resort.
    chop ($__pwd = `pwd`);
  }

  return $__pwd;
}

sub set_defaults
{
   # $profile = 1;
   $testee = "GNU make";
   $make_path = "make";
   $tmpfilesuffix = "mk";
   $pwd = &get_this_pwd;
}

sub set_more_defaults
{
   local($string);
   local($index);

   # find the type of the port.  We do this up front to have a single
   # point of change if it needs to be tweaked.
   #
   # This is probably not specific enough.
   #
   if ($osname =~ /Windows/i || $osname =~ /MINGW32/i || $osname =~ /CYGWIN_NT/i) {
     $port_type = 'W32';
   }
   # Bleah, the osname is so variable on DOS.  This kind of bites.
   # Well, as far as I can tell if we check for some text at the
   # beginning of the line with either no spaces or a single space, then
   # a D, then either "OS", "os", or "ev" and a space.  That should
   # match and be pretty specific.
   elsif ($osname =~ /^([^ ]*|[^ ]* [^ ]*)D(OS|os|ev) /) {
     $port_type = 'DOS';
   }
   # Check for OS/2
   elsif ($osname =~ m%OS/2%) {
     $port_type = 'OS/2';
   }

   # VMS has a GNV Unix mode or a DCL mode.
   # The SHELL environment variable should not be defined in VMS-DCL mode.
   elsif ($osname eq 'VMS' && !defined $ENV{"SHELL"}) {
     $port_type = 'VMS-DCL';
   }
   # Everything else, right now, is UNIX.  Note that we should integrate
   # the VOS support into this as well and get rid of $vos; we'll do
   # that next time.
   else {
     $port_type = 'UNIX';
   }

   # On DOS/Windows system the filesystem apparently can't track
   # timestamps with second granularity (!!).  Change the sleep time
   # needed to force a file to be considered "old".
   $wtime = $port_type eq 'UNIX' ? 1 : $port_type eq 'OS/2' ? 2 : 4;

   print "Port type: $port_type\n" if $debug;
   print "Make path: $make_path\n" if $debug;

   # Find the full pathname of Make.  For DOS systems this is more
   # complicated, so we ask make itself.
   if ($osname eq 'VMS') {
     $port_type = 'VMS-DCL' unless defined $ENV{"SHELL"};
     # On VMS pre-setup make to be found with simply 'make'.
     $make_path = 'make';
   } else {
     my $mk = `sh -c 'echo "all:;\@echo \\\$(MAKE)" | $make_path -f-'`;
     chop $mk;
     $mk or die "FATAL ERROR: Cannot determine the value of \$(MAKE):\n
'echo \"all:;\@echo \\\$(MAKE)\" | $make_path -f-' failed!\n";
     $make_path = $mk;
   }
   print "Make\t= '$make_path'\n" if $debug;

   my $redir2 = '2> /dev/null';
   $redir2 = '' if os_name eq 'VMS';
   $string = `$make_path -v -f /dev/null $redir2`;

   $string =~ /^(GNU Make [^,\n]*)/;
   $testee_version = "$1\n";

   my $redir = '2>&1';
   $redir = '' if os_name eq 'VMS';
   $string = `sh -c "$make_path -f /dev/null $redir"`;
   if ($string =~ /(.*): \*\*\* No targets\.  Stop\./) {
     $make_name = $1;
   }
   else {
     $make_path =~ /^(?:.*$pathsep)?(.+)$/;
     $make_name = $1;
   }

   # prepend pwd if this is a relative path (ie, does not
   # start with a slash, but contains one).  Thanks for the
   # clue, Roland.

   if (index ($make_path, ":") != 1 && index ($make_path, "/") > 0)
   {
      $mkpath = "$pwd$pathsep$make_path";
   }
   else
   {
      $mkpath = $make_path;
   }

   # If srcdir wasn't provided on the command line, see if the
   # location of the make program gives us a clue.  Don't fail if not;
   # we'll assume it's been installed into /usr/include or wherever.
   if (! $srcdir) {
       $make_path =~ /^(.*$pathsep)?/;
       my $d = $1 || '../';
       -f "${d}gnumake.h" and $srcdir = $d;
   }

   # Not with the make program, so see if we can get it out of the makefile
   if (! $srcdir && open(MF, "< ../Makefile")) {
       local $/ = undef;
       $_ = <MF>;
       close(MF);
       /^abs_srcdir\s*=\s*(.*?)\s*$/m;
       -f "$1/gnumake.h" and $srcdir = $1;
   }

   # Get Purify log info--if any.

   if (exists $ENV{PURIFYOPTIONS}
       && $ENV{PURIFYOPTIONS} =~ /.*-logfile=([^ ]+)/) {
     $pure_log = $1 || '';
     $pure_log =~ s/%v/$make_name/;
     $purify_errors = 0;
   }

   $string = `sh -c "$make_path -j 2 -f /dev/null $redir"`;
   if ($string =~ /not supported/) {
     $parallel_jobs = 0;
   }
   else {
     $parallel_jobs = 1;
   }

   %FEATURES = map { $_ => 1 } split /\s+/, `sh -c "echo '\\\$(info \\\$(.FEATURES))' | $make_path -f- 2>/dev/null"`;

   # Set up for valgrind, if requested.

   $make_command = $make_path;

   if ($valgrind) {
     my $args = $valgrind_args;
     open(VALGRIND, "> valgrind.out")
       || die "Cannot open valgrind.out: $!\n";
     #  -q --leak-check=yes
     exists $ENV{VALGRIND_ARGS} and $args = $ENV{VALGRIND_ARGS};
     $make_path = "valgrind --log-fd=".fileno(VALGRIND)." $args $make_path";
     # F_SETFD is 2
     fcntl(VALGRIND, 2, 0) or die "fcntl(setfd) failed: $!\n";
     system("echo Starting on `date` 1>&".fileno(VALGRIND));
     print "Enabled valgrind support.\n";
   }
}

sub setup_for_test
{
  $makefile = &get_tmpfile;
  if (-f $makefile) {
    unlink $makefile;
  }

  # Get rid of any Purify logs.
  if ($pure_log) {
    ($pure_testname = $testname) =~ tr,/,_,;
    $pure_testname = "$pure_log.$pure_testname";
    system("rm -f $pure_testname*");
    print("Purify testfiles are: $pure_testname*\n") if $debug;
  }
}

exit !&toplevel;
