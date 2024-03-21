// Barre de progression thread-safe, pour afficher la progression d'une génération
#include <cstdio>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

#include <mutex>
class ProgressBar{
	public: 	
	int progression = 0;
	int max;
	int displayWidth;
	float displayed = 0;
	std::mutex mProg;

	ProgressBar(int max, int displayWidth){
		this->displayWidth = displayWidth;
		this->max = max;
	};
	// largeur du terminal
	ProgressBar(int max){
		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

		this->displayWidth = w.ws_col;
		this->max = max;
	};
	~ProgressBar(){
		printf("\33[2K\r");
		printf("\033[0m"); 
		fflush(stdout);
	};

	void step(int prog = 1, int color = 0){
		mProg.lock();
		progression+=prog;
		float futureDisplay = ((float)progression)/(float)max*(float)displayWidth;
		for(int i = 0; i < floor(futureDisplay - displayed); i++){
			printf("\033[38;5;%dm", color+1); 
			std::cout <<"_";
			displayed++;
		}
		fflush(stdout);
		mProg.unlock();
	};
};
