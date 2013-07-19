#include <fstream>
#include <iostream>
#include <string>
#include <vector>
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


struct Configs {
	bool dots = false;
};


class Command {
public:
	virtual void execute(Configs& configs) =0;
};

class Analyzer : public Command {
protected:
	void analyze(istream& is, Configs& configs) {
		::analyze(is, configs.dots);
	}
};

class FileAnalyzer : public Analyzer {
private:
	string filename;
public:
	FileAnalyzer(const string& filename) : filename(filename) {}
	void execute(Configs& configs) {
		ifstream ifs(filename.c_str());
		if(ifs) {
			cout << "Analyzing file " << filename << endl;
			Analyzer::analyze(ifs, configs);
		} else {
			cerr << "Can't open file " << filename << '!' << endl;
		}
	}
};

class StdinAnalyzer : public Analyzer {
public:
	void execute(Configs& configs) {
		cout << "Analyzing standard input" << endl;
		Analyzer::analyze(cin, configs);
	}
};

class SetDots : public Command {
private:
	bool dots;
public:
	SetDots(bool dots) : dots(dots) {}
	void execute(Configs& configs) {
		configs.dots = dots;
	}
};


typedef vector<Command*> Commands;

Commands processArguments(const char** argBegin, const char** argEnd) {
	Commands commands;
	int fileCount = 0;
	for(const char** arg_iterator = argBegin; arg_iterator != argEnd; arg_iterator++) {
		const string arg = *arg_iterator;
		if(arg == "--dots") {
			commands.push_back(new SetDots(true));
		} else if(arg == "--no-dots") {
			commands.push_back(new SetDots(false));
		} else {
			const string& filename = arg;
			commands.push_back(new FileAnalyzer(filename));
			fileCount++;
		}
	}
	if(fileCount == 0) {
		commands.push_back(new StdinAnalyzer());
	}
	
	return commands;
}


int main(int argc, const char* argv[]) {
	Commands commands = processArguments(argv+1, argv+argc);
	
	Configs configs;
	for(Command* command : commands) {
		command->execute(configs);
	}
}
