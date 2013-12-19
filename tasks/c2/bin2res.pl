#!/usr/bin/perl -w
#
# Simple command line tool to embed multiple (binary or text) files
# into C source code.
#
# Usage:
#  perl bin2res.pl [-f <func>] [-i] [<file1> ... <fileN>]
#
# -f <func>   Allows the use to specify an alternate resource access
#             function (the default is "EmbeddedResourceLookup")
#
# -i          Exposes the names of embedded resources using a special
#             ".index" resource entry.

use strict;
use Getopt::Std qw(getopts);

# Regular expression for whitelisting of filenames
my $regex_filename = qr{^[a-z0-9_]+(?:\.[a-z0-9_]+)*(?:\/[a-z0-9_]+(?:\.[a-z0-9_]+)*)*$}i;

# Regular expression for whitelisting of C function names
my $regex_funcname = qr{^[a-z_][a-z0-9_]*$}i;

#----------------------------------------------------------------------
# Command line
#

my %options = ();
getopts("f:i", \%options) or die "ERROR: Unexpected command line options";

# Name of the C function
my $lookup_function = "EmbeddedResourceLookup";
$lookup_function = $options{'f'} if defined $options{'f'};

# Generate special ".index" entry
my $generate_index = defined $options{'i'};

#----------------------------------------------------------------------
# Input validation and name index generation
#

# Check the lookup function
die "ERROR: function name '$lookup_function' was rejected by the whitelisting pattern"
    unless ($lookup_function =~ $regex_funcname);

# Check the file names against regex_filename and assign a unique index
# for each file.
my %files;
my $filecount = 0;

my $name_index = "";

foreach my $filename (@ARGV) {
    # Check the filename
    die "ERROR: filename '$filename' was rejected by the whitelisting pattern"
	unless ($filename =~ $regex_filename);

    # Check that the file exists
    die "ERROR: filename '$filename' does not reference a regular file"
	unless (-f $filename);

    # Check for duplicate names
    die "ERROR: filename '$filename' has been specified more than once.\n"
	if (exists $files{$filename});

    # Record in the hash
    $files{$filename} = {'index' => $filecount};
    $filecount += 1;
}

# Sort the data file names in command line order
my @datafiles = sort { $files{$a}{'index'} <=> $files{$b}{'index'} } keys %files;

#----------------------------------------------------------------------
# Part 1: Includes and definitions
#
print STDOUT <<EOF;
/*
 * Automatically generated by bin2res.pl
 *
 * WARNING: Changes to this file will be list when the bin2res.pl tool
 * is invoked again.
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

const void* ${lookup_function}(size_t *length, const char *name)
{
  size_t n;
EOF

#----------------------------------------------------------------------
# Part 2: Resource names
#
print STDOUT "  static const const char index[] = \"";

my $name_pos = 0;
my $line_len = 70;

foreach my $filename (sort (keys %files)) {
    print STDOUT $filename;
    print STDOUT "\\0";

    $files{$filename}{'name_offset'} = $name_pos;
    $name_pos += length($filename) + 1;

    $line_len += length($filename) + 2;
    if ($line_len >= 70) {
	print STDOUT "\"\n    \"";
	$line_len = 0;
    }
}

print STDOUT "\";\n";

#----------------------------------------------------------------------
# Part 3: Resource data
#
# Emit one byte array for all input files. (Note that we currently do not use
# C99-style compound literals to increase portability accross compilers)
#
# We insert an extra '\0' at the end of each input file to allow simple
# interaction with C string literals in text files.
#

print STDOUT "  static const unsigned char data\[\] = {\n";

my $data_pos = 0;

# Create the resource records
foreach my $filename (@datafiles) {
    # Print the raw file content
    print STDOUT "    /* $filename */\n";

    # Record start of file
    $files{$filename}{'data_offset'} = $data_pos;

    # Read the file content
    my $handle;
    open ($handle, "<:raw", $filename);

    my $column = 0;

    while (defined (my $c = getc($handle))) {
	if ($column == 0) {
	    print STDOUT "    ";
	}

	printf STDOUT "0x%02x,", ord($c);
	$column += 1;

	if ($column >= 16) {
	    print STDOUT "\n";
	    $column = 0;
	}

	# Advance data position
	$data_pos += 1;
    }

    if ($column > 0) {
	print STDOUT "\n";
    }

    # Record size of file
    $files{$filename}{'data_size'} = $data_pos - $files{$filename}{'data_offset'};

    # Extra NUL and advance data position
    print STDOUT "    0x00,\n";
    $data_pos += 1;

    close ($handle);
}

print STDOUT "  };\n\n";

#----------------------------------------------------------------------
# Part 4: Resource index
#
print STDOUT <<EOF;
  /* resource data index */
  static const struct {
    size_t name_offset;
    size_t data_offset;
    size_t data_length;
  } resources[$filecount] = {
EOF

foreach my $filename (@datafiles) {
  my $name_offset = $files{$filename}{'name_offset'};
  my $data_offset = $files{$filename}{'data_offset'};
  my $data_size   = $files{$filename}{'data_size'};
  printf STDOUT "    { %6u, %6u, %6u }, /* %s */\n", $name_offset, $data_offset, $data_size, $filename;
}

print STDOUT <<EOF;
  };

  if (!name) {
    if (length) {
      *length = 0;
    }
    return NULL;
  }

EOF


#----------------------------------------------------------------------
# Part 5: Resource main lookup
#
print STDOUT <<EOF;
  /* main lookup */
  for (n = 0; n < $filecount; ++n) {
    if (strcmp(index + resources[n].name_offset, name) == 0) {
      if (length) {
        *length = resources[n].data_length;
      }
      return data + resources[n].data_offset;
    }
  }

EOF

#----------------------------------------------------------------------
# Part 6: Optional access to resource index (".index")
#
if ($generate_index) {
print STDOUT <<EOF;
  /* .index */
  if (strcmp(".index", name) == 0) {
    if (length) {
      *length = sizeof(index) - 1U;
    }

    return index;
  }

EOF
}

#----------------------------------------------------------------------
# Part 5: Function and file trailer
#
print STDOUT <<EOF;
  /* no matching entry found */
  if (length) {
    *length = 0;
  }
  return NULL;
}
EOF
