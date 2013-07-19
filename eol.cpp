#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

enum {
	CR = 13,
	LF = 10,
	TAB = 9,
};


bool isText(unsigned char ch) {
	return ch == CR
	   ||  ch == LF
	   ||  ch == TAB
	   ||  ch >= ' '
	   ;
}


const char* const NO_COLOR = "\e[0m";
const char* const CR_COLOR = "\e[31;1m";
const char* const LF_COLOR = "\e[32;1m";
const char* const CRLF_COLOR = "\e[34;1m";


void analyze(istream& is, bool dots = false) {
	int chars = 0;
	
	int crlfCount = 0;
	int crCount = 0;
	int lfCount = 0;
	int nonTextCount = 0;
	
	bool wasCR = false;
	bool wasEOL = false;
	while(true) {
		int ch = is.get();
		if(ch == EOF) {
			break;
		} else {
			chars++;
			if(!isText(ch)) {
				nonTextCount++;
			}
			
			if(ch == CR) {
				crCount++;
				if(dots) { cout << CR_COLOR << '.' << flush; }
			} else if(ch == LF) {
				if(wasCR) {
					crCount--;
					crlfCount++;
					if(dots) { cout << "\b" << CRLF_COLOR << '.' << flush; }
				} else {
					lfCount++;
					if(dots) { cout << LF_COLOR << '.' << flush; }
				}
			}
			
			wasCR = (ch == CR);
			wasEOL = (ch == CR  ||  ch == LF);
		}
	}
	
	if(dots) { cout << NO_COLOR << endl; }
	
	if(dots) { cout << LF_COLOR << '*' << NO_COLOR; }
	cout << "LF / '\\n' / 10 / 0x0A (Unix) : " << lfCount << " occurrences" << endl;
	
	if(dots) { cout << CR_COLOR << '*' << NO_COLOR; }
	cout << "CR / '\\r' / 13 / 0x0D (Apple): " << crCount << " occurrences" << endl;
	
	if(dots) { cout << CRLF_COLOR << '*' << NO_COLOR; }
	cout << "CR+LF                 (Win)  : " << crlfCount << " occurrences" << endl;
	
	cout << "Total terminators: " << (lfCount + crCount + crlfCount) << endl;
	cout << "Total chars: " << chars << endl;
	if(nonTextCount) {
		cout << "Probably not a text file! (" << nonTextCount << " binary chars found)" << endl;
	}
	if(!wasEOL) {
		cout << "Does not end with a line terminator" << endl;
	}
	
	cout << endl;
}


int main(int argc, const char* argv[]) {
	int fileCount = 0;
	bool dots = false;
	
	for(int i = 1; i < argc; i++) {
		const string arg = argv[i];
		if(arg == "--dots") {
			dots = true;
		} else if(arg == "--no-dots") {
			dots = false;
		} else {
			const string& filename = arg;
			ifstream ifs(filename.c_str());
			if(ifs) {
				cout << "Analyzing file " << filename << endl;
				analyze(ifs, dots);
			} else {
				cerr << "Can't open file " << filename << '!' << endl;
			}
			fileCount++;
		}
	}
	
	if(fileCount == 0) {
		cout << "Analyzing standard input" << endl;
		analyze(cin);
	}
}

