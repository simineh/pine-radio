#!/bin/perl

# ������ ���� � ������� ����� � �� ���� ��������� ������
# ����� � ����������� tmpl, �� �� ������ ������� �����
# c ����� �� ������, �� ��� ���������� tmpl.
# �������� ��� ��������� BUILD_NUMBER �� ����� ������,
# �.�. ����� ������� ������� �� ������� �����

use File::Find;
use Cwd;

my $scriptName = "build_incrementor";

my $buildNumberFilename = "buildnumber";
my $buildNumberMarker = "BUILD_NUMBER";

my $workingDirectory = $ARGV[0];
my $currentDirectory = getcwd;

print "$scriptName: current directory is $currentDirectory\n";
print "$scriptName: working directory is $workingDirectory\n";

my $buildNumberPath = "$workingDirectory/$buildNumberFilename";

my $buildNumber;

if (-e $buildNumberPath)
{
	open BUILDNUMBER, "<", $buildNumberPath;
	$buildNumber =  <BUILDNUMBER>;
	close BUILDNUMBER;
	$buildNumber++;
} else {
	open BUILDNUMBER, ">", $buildNumberPath;
	print BUILDNUMBER 0;
	close BUILDNUMBER;
	$buildNumber = 0;
}

print "$scriptName: build number is $buildNumber\n";

sub applyTemplate{
	if($_ =~ /\.tmpl$/i) {
		my $templateFileName = $_;
		my $fileName = substr($templateFileName, 0, length($_) - 5);
		
		open TEMPLATEFILE, "<", $templateFileName;
		open RESULTFILE, ">", $fileName;
		while (<TEMPLATEFILE>) {
			my $resultLine = $_;
			$resultLine =~ s/$buildNumberMarker/$buildNumber/g;
			print RESULTFILE "$resultLine";
		}
		close RESULTFILE;
		close TEMPLATEFILE;
	}
}

find(\&applyTemplate, $workingDirectory);

open BUILDNUMBER, ">", $buildNumberPath;
print BUILDNUMBER $buildNumber;
close BUILDNUMBER;

print "$scriptName: done\n";

