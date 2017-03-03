#include "my.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
 #include <unistd.h>
 #include <fcntl.h>
 #include <stdio.h>
 #include <sys/stat.h>

int jercnt = 0;
pid_t pid = -1;
int histLen=0; /*amount of things in history */
char* originPath = NULL; /*path for history file */
struct s_node* head=NULL; /*head of linked list of history */

void exitShell();
void writeToFile();
void readin();

void ctrlc(int sig){
	
}

/*exit program*/
void exitShell(){
  addstr("Leaving shell...\n");
  refresh();
  endwin();
  writeToFile();

  exit(0);
}

/*exit program, printing error*/
void exitError(char* message){
	addstr(message);
	refresh();
	endwin();
	my_str(message);
	exit(1);
	
}

/*insert a character at a specified index in a string */
void insertChar(char* s, char c, int i, int l){
	/*insert char c into String s with length l at index i*/
	int counter;
	for(counter = l; counter>=0; counter--){
		if (counter >i){
			s[counter] = s[counter-1];
		}else if(counter ==i){
			s[counter] = c;
			break;
		}
	}
}

/*delete character at specified index from a string */
void removeChar(char* s, int i, int l){
	/*remove char at index i in String s with length l */
	int counter;
	for(counter = 0; counter<l; counter++){
		if (counter >=i){
			s[counter] = s[counter+1];
		}
	}
}

/*read the .nsmshistory file into memory */
void readin(){
	int o;
	int r = 1;
	char c[1];
	int maxSize = 1024;
	char* b =(char*) malloc(1024);
	int counter = 0;
	
	/*open the file, or create it if it doesn't exist */
	if(( o = open(originPath, O_RDONLY| O_CREAT,  S_IRUSR | S_IWUSR)) <0){
		perror("Couldn't open/create file");
		exitError("Could not open/create file");;
	}
	while(r>0){
		r = read(o,c,1);
		if(*c != '\n'){
			if(counter >= maxSize-1){
				char* b1 = my_strconcat(b,c);
				free(b);
				b = b1;
				counter++;
			}else{
				b[counter]=*c;
				counter++;
			}
		}else{
			b[counter] = '\0';
			if(counter >0){
				add_elem(strdup(b),&head);
				histLen++;
				counter = 0;
			}
		}
	 }
	free(b);
	close(o);
}

/*write all history data into the .nsmshistory file */
void writeToFile(){
	int len = count_s_nodes(head);
	int i;
	int o;
	/*open the file, or create it if it doesn't exist */
	if(( o = open(originPath, O_WRONLY | O_CREAT,  S_IRUSR | S_IWUSR)) <0){
		perror("Couldn't open/create file");
		exitError("Could not open/create file");
	}
	for(i = len-1; i>=0; i--){
		char* elem = (char *)elem_at(head,i);
		int size = my_strlen(elem);
		char* temp = "\n";
		write(o,elem,size);
		write(o,temp,1);
	}
	close(o);
}

/*reinitialize ncurses after calling exec */
void reinit(){
	keypad(stdscr, TRUE);
	raw();
	noecho();
	scrollok(stdscr,TRUE);
	idlok(stdscr, TRUE);
	refresh();
}

/*unitialize ncurses before calling exec */
void uninit(){
	keypad(stdscr, FALSE);
	noraw();
	nocbreak();
	echo();
	scrollok(stdscr,TRUE);
	idlok(stdscr, TRUE);
	refresh();
}

/*print current working directory*/
int printCur(){
    char pwd[4096];
	char* c1;
	char* c2;
	char* sName = "MINISHELL: ";
	char* pEnd = " $: ";
	int len;
    getcwd(pwd, sizeof(pwd));
	c1 = my_strconcat(pwd, pEnd);
	c2 = my_strconcat(sName, c1);
	len = my_strlen(c2);
	if(has_colors()){
		attron(COLOR_PAIR(7));
		addstr(sName);
		attron(COLOR_PAIR(3));
		addstr(pwd);
		attron(COLOR_PAIR(7));
		addstr(pEnd);
		attroff(COLOR_PAIR(7));
	}else {
		addstr(c2);
	}
	free(c1);
	free(c2);
	return len;
}
/*change directory*/
int cd(char* path){
    return chdir(path);
}

void init(){
	initscr();
	keypad(stdscr, TRUE);
	raw();
	noecho();
	scrollok(stdscr,TRUE);
	idlok(stdscr, TRUE);
	refresh();
	/*save .nsmshistory in the users home directory*/
	originPath = my_strconcat(getenv("HOME"),"/.nsmshistory"); 
	readin(); /*read .nsmshistory into memory */
	if(has_colors()){	
		start_color();
		init_pair(1, COLOR_BLACK, COLOR_BLACK);
		init_pair(2, COLOR_RED, COLOR_BLACK);
		init_pair(3, COLOR_GREEN, COLOR_BLACK);
		init_pair(4, COLOR_YELLOW, COLOR_BLACK);
		init_pair(5, COLOR_BLUE, COLOR_BLACK);
		init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(7, COLOR_CYAN, COLOR_BLACK);
		init_pair(8, COLOR_WHITE, COLOR_BLACK);
	}		
}

/*error message*/
void jerror(char* msg, char* var){
	if(has_colors()){
		attron(COLOR_PAIR(2));
	}
    if(jercnt == 0){
        addstr("Error: invalid input -");
    }else if(jercnt == 1){
        addstr("404: Error not found:");
    }else if(jercnt == 2){
        addstr("Error: Please enter bank account information because we");
    }else if(jercnt == 3){
        addstr("Error: an error has occurred: ");
    }else if(jercnt == 4){
        addstr("418: I'm a teapot -  ");
	}else if(jercnt == 5){
        addstr("Error: Input accepted: ");
    }else if(jercnt == 6){
        addstr("Error: System not responding, press alt + f4 because we");
    }else if(jercnt == 7){
        addstr("Error: Your free trial has expired because we");
    }else if(jercnt == 8){
        addstr("Error: Please restart computer to continue because we");
    }else{
        addstr("Error: Failed to connect to printer - ");
    }
	addstr(msg);
	addstr(var);
	addch('\n');
    jercnt = (jercnt + 1) % 10;
	if(has_colors()){
		attroff(COLOR_PAIR(2));
	}
}

/*execute program*/
char* execute(int type, char** paramv){
	int c2pfd[2];
    char* program = paramv[0];
	char* buffer = (char*)malloc(2);
	fd_set readfds;
	pipe(c2pfd);
	uninit();
    if((pid = fork()) < 0){
        perror("Error Forking"), exit(1);
    }else if(pid == 0){
		close(c2pfd[0]);
		dup2(c2pfd[1],1);
		dup2(c2pfd[1],2); 
		close(c2pfd[1]);     
        execvp(program, paramv);
        exit(-1);
    }else{
        int status;
		int retval;
		int n;
		int counter=0;
		int sizeToRead = 1;	
		close(c2pfd[1]);
		while(1){
			FD_ZERO(&readfds);
			FD_SET(c2pfd[0], &readfds);
			FD_SET(0, &readfds);
			select( c2pfd[0] + 1 , &readfds , NULL , NULL , NULL);
			if(FD_ISSET(0, &readfds)){
				refresh();
			}
			if(FD_ISSET(c2pfd[0], &readfds)){
					
				
				n=read(c2pfd[0], buffer+counter, 1);
				if(type != 2){
					if(n<=0){
						break;
					}else {
						buffer[n]= '\0';
						addstr(buffer);
						refresh();
					}
				}else{ /*don't print, just save in buffer */
					int len;
					if(n<=0){
						buffer[counter] = '\0';
						break;
					}else{
						counter++;
						buffer[counter] = '\0';
						len= my_strlen(buffer);
						if(len>=sizeToRead-1){
							char* temp;
							sizeToRead*=2;
							temp = (char*)realloc(buffer,sizeToRead );
							if(temp==NULL){
								perror("realloc failed");
								exit(1);
							}
							buffer = temp;
						}	
					}	
				}	
			}
		}
		refresh();
        waitpid(pid, &status,0);
		retval = WEXITSTATUS(status);
        if(retval !=0){
            jerror(" cannot find ", program);
        }
		reinit();
		free(paramv);
		close(c2pfd[0]);
    }
    return buffer;
}
/*display information, return to terminal*/
void help(){
    addstr("\nYou must be truly desperate if you've come to me for help\n");
    addstr("\ncd: change directory\nuse .. to move to lower directory \n use name of directory to move there\n");
    addstr("\nexit: leaves the shell\n");
    addstr("\nhelp: displays helpful information\n\n");
}

/*move cursor to the right to position m starting at position c */
void moveRight(int maxX,int c, int m){
	int x;
	int y;
	while(c<m){
		getyx(stdscr,y,x);
		if(x<maxX-1){
			move(y,x+1);
			c++;
		}else if (x>=maxX-1){
			move(y+1,0);
			c++;
		}
	}
}
int main(){
    char* buffer = NULL; /*input buffer */
    char* temp = NULL;
	int pLen = 0; /*length of prompt */
    int sizeToRead = 1024; /*same size as buffer allocation */
	int bLen = 0; /*running length of buffer*/
	/*int reachedBottom = 0; *//*checks if reached bottom of terminal window */
	char* copyC = NULL; /* "clipboard" contents */
	int flagUW = 2; /*flag to determine change between copy line and copy word */
	clear(); /*clear current screen */
	init(); /*initialize ncurses */
	signal(SIGINT, ctrlc);
	buffer = (char*)malloc(sizeToRead); /*same size as sizeToRead */
	
    while(1){
		int x;
		int y;
		int maxX;
		int maxY;
		int lIndex=-1; /*index in history list */
		int yDisp; /*y displacement value */
		int xDisp; /*x displacement value */
		int strPos = 0; /*position in string */
		int origY; /*starting Y position (end of prompt) */
		int origX; /*starting x position (end of prompt) */
		char* tmpBuffer = NULL;
		char* start = NULL;
		getyx(stdscr,y,x);
		getmaxyx(stdscr, maxY,maxX);
		move(y,0);
        pLen=printCur();
		yDisp = pLen/maxX;
		xDisp = pLen % maxX;
		origY = y+yDisp;
		if(origY>=maxY-1){
			
			origY = maxY -1;
			/*reachedBottom = 1;*/
		}
		origX = x+xDisp;
		move(origY, x+xDisp);

		bLen=0;
		buffer[0] = '\0';
		memset(buffer, 0, sizeToRead);
		refresh();
		while(1){
			int key = wgetch(stdscr);
			getyx(stdscr,y,x);
			
			/*if we overflow buffer, expand it */
			if (bLen >= sizeToRead-1){
				sizeToRead*=2;
				temp = (char*)realloc(buffer, sizeToRead);
				if(temp ==NULL){
					perror("Realloc failed"), exit(1);
				}else {
					buffer = temp;
				}
			}
			if(key == 27){ /*escape */
				int ch;
				timeout(.01);
				ch= getch();
				if(ch ==-1){
					endwin();
					exit(0);
					break;
				}
			}else if(key == 12){ /* CTLR + L*/
				int count = 0;
				origY =0;
				clear();
				printCur();
				move(origY,origX);
				addstr(buffer);
				move(origY,origX);
				moveRight(maxX,count, strPos);
			}else if(key==1){ /*CTRL + A */
				move(origY,origX);
				strPos = 0;
			}else if(key ==5){ /*CTRL +E */
				move(origY,origX);
				moveRight(maxX, 0, bLen);
				strPos = bLen;
			}else if(key==23){/*CTRL + W */
				int counter = strPos;
				char* temp;
				int i;
				int size;
				int cnt =0;
				int restorePos;
				if(strPos >0){
					/*move one spot left */
					if(strPos>0 && x>0){
						move(y,x-1);
						strPos--;
					}else if (strPos >=0 && x<=0){
						move(y-1,maxX-1);
						strPos--;
					}	
					counter--;
					
					/*bypass all spaces */
					while(buffer[counter] ==' ' && counter >=0){
						if(strPos>0 && x>0){
							move(y,x-1);
							strPos--;
						}else if (strPos >=0 && x<=0){
							move(y-1,maxX-1);
							strPos--;
						}	
						counter--;
						cnt++;
					}
					
					/*count positions backwards to cut */
					while(buffer[counter] != ' ' && counter >=0){
						counter--;
					}
					size = (strPos-counter)+cnt;
					restorePos = counter;
					temp = (char*)malloc(size+1);
					counter++;
					/*put characters from buffer into temp variable*/
					for(i=0;i<size; i++){
						if(i<=(size-cnt)){
							temp[i] = buffer[counter];
						}
						removeChar(buffer,counter,bLen );
						bLen--;
					}
					temp[size] = '\0';
					move(origY,origX);
					addstr(buffer);
					move(origY,origX);
					moveRight(maxX, 0, bLen+size);
					strPos = bLen+size;
					
					/*remove all extra characters from screen*/
					for(i = 0; i <= size; i++ ){
						getyx(stdscr,y,x);
						delch();
						if(strPos>0 && x>0){
							move(y,x-1);
							strPos--;
						}else if (strPos >=0 && x<=0){
							move(y-1,maxX-1);
							strPos--;
						}
					}		
					strPos = restorePos+1;
					move(origY,origX);
					moveRight(maxX,0,strPos);
					if(flagUW == 1){
						flagUW = 0;
						free(copyC);
						copyC = my_strdup(temp);
					}else if(copyC !=NULL){
						char* t;
						t = my_strconcat(my_strdup(temp),copyC);
						free(copyC);
						copyC = t;
					}else if (copyC == NULL){
						copyC = my_strdup(temp);
						flagUW = 0;
					}
					free(temp);
					buffer[bLen] = '\0';
				}
			}else if(key ==21){ /*CTRL + U */
				if(strPos>0){
					int i;
					int newLen;
					if(copyC != NULL){
						free(copyC);
					}
					flagUW = 1;
					buffer[bLen] = '\0';
					newLen = bLen-strPos;
					copyC = (char*)malloc(strPos+1);
					my_strncpy(copyC,buffer,strPos);
					copyC[strPos] = '\0';
					moveRight(maxX, strPos, bLen);
					
					for( i = 0; i<strPos; i++){
						removeChar(buffer,0,bLen);
					}
					for(i = 0; i <= bLen; i++ ){
						getyx(stdscr,y,x);
						delch();
						 if(strPos>0 && x>0){
							 move(y,x-1);
							 strPos--;
						 }else if (strPos >=0 && x<=0){
							 move(y-1,maxX-1);
							 strPos--;
						 }
					}
					move(origY,origX);
					addstr(buffer);
					move(origY,origX);
					bLen =newLen;
					buffer[bLen] = '\0';
					strPos = 0;
				}
				
			}else if(key == 25){ /*YANK!!!! */
				if(copyC !=NULL){
					int i = 0;
					int l= my_strlen(copyC);
					int yOffset;
					for(i = 0; i < l; i++){
						if (bLen >= sizeToRead-1){
							sizeToRead*=2;
							temp = (char*)realloc(buffer, sizeToRead);
							if(temp ==NULL){
								perror("Realloc failed"), exit(1);
							}else {
								buffer = temp;
							}
						}
						bLen++;
						insertChar(buffer,copyC[i],strPos,bLen);
						strPos++;
					}
					yOffset= (pLen%maxX+bLen)/maxX;
					move(origY,origX);
					addstr(buffer);
					yOffset= (pLen%maxX+bLen)/maxX;
					if(yOffset>0&& yOffset+origY > maxY-1){
						origY = (maxY-1) - yOffset;
					}
					move(origY, origX);
					strPos = bLen;
					moveRight(maxX, 0, bLen);
				}
			}else if(key==KEY_RESIZE){
				/*int yOffset;
				int i;
				int y;
				int x;
				moveRight(maxX,strPos, bLen);
				strPos = bLen+pLen;
				for(i = 0; i <= bLen+pLen; i++ ){
					getyx(stdscr,y,x);
					delch();
					 if(strPos>0 && x>0){
						 move(y,x-1);
						 strPos--;
					 }else if (strPos >=0 && x<=0){
						 move(y-1,maxX-1);
						 strPos--;
					 }
				}
				getyx(stdscr,y,x);*/
				getmaxyx(stdscr, maxY,maxX);
				/*move(y,0);
				pLen=printCur();
				yDisp = pLen/maxX;
				xDisp = pLen % maxX;
				origY = y+yDisp;
				if(origY>=maxY-1){
					origY = maxY -1;
				}
				origX = x+xDisp;
				move(origY, origX);
				addstr(buffer);
				yOffset= (pLen%maxX+bLen)/maxX;
				if(yOffset>0&& yOffset+origY > maxY-1){
					origY = (maxY-1) - yOffset;
				}
				move(origY, origX);
				strPos = bLen;
				moveRight(maxX, 0, bLen);*/
			}else if (key == KEY_BACKSPACE || key == KEY_DC){
				if(strPos>0){
					int count = 0;
					if(x>0 && key == KEY_BACKSPACE){
						move(y,x-1);
						strPos--;
					}else if (x <=0 && key == KEY_BACKSPACE){
						move(y-1,maxX-1);
						strPos--;
					}
					removeChar(buffer,strPos,bLen);
					bLen--;	
					buffer[bLen] = '\0';
					move(origY,origX);
					addstr(buffer);
					delch();		
					move(origY,origX);
					moveRight(maxX, count, strPos);
				}
			}else if(key==KEY_DOWN){
				int i;
				int yOffset;
				move(origY,origX);
				if(lIndex>=0){
					lIndex--;
				}
				moveRight(maxX, 0, bLen);
				for(i = 0; i <= bLen; i++ ){
					getyx(stdscr,y,x);
					delch();
					 if(strPos>0 && x>0){
						 move(y,x-1);
						 strPos--;
					 }else if (strPos >=0 && x<=0){
						 move(y-1,maxX-1);
						 strPos--;
					 }
				}
				if(lIndex>=0){
					my_strcpy(buffer, (char*)elem_at(head, lIndex));
				}
				else {*buffer = '\0';}
				bLen = my_strlen(buffer);
				yOffset= (pLen%maxX+bLen)/maxX;
				move(origY,origX);
				addstr(buffer);
				yOffset= (pLen%maxX+bLen)/maxX;
				if(yOffset>0&& yOffset+origY > maxY-1){
					origY = (maxY-1) - yOffset;
				}
				move(origY, origX);
				strPos = bLen;
				moveRight(maxX, 0, bLen);
				
			} else if(key==KEY_UP){
				int i;
				int yOffset;
				move(origY,origX);
				if(lIndex<histLen-1){
					lIndex++;
				}
				moveRight(maxX, 0, bLen);
				for(i = 0; i <= bLen; i++ ){
					getyx(stdscr,y,x);
					delch();
					 if(strPos>0 && x>0){
						 move(y,x-1);
						 strPos--;
					 }else if (strPos >=0 && x<=0){
						 move(y-1,maxX-1);
						 strPos--;
					 }
				}
				my_strcpy(buffer, (char*)elem_at(head, lIndex));
				bLen = my_strlen(buffer);
				yOffset= (pLen%maxX+bLen)/maxX;
				move(origY,origX);
				addstr(buffer);
				yOffset= (pLen%maxX+bLen)/maxX;
				if(yOffset>0 && yOffset+origY > maxY-1){
					origY = (maxY-1) - yOffset;
				}
				move(origY, origX);
				strPos = bLen;
				moveRight(maxX, 0, bLen);
				
			}else if(key == KEY_LEFT){
				 if(strPos>0 && x>0){
					 move(y,x-1);
					 strPos--;
				 }else if (strPos >=0 && x<=0){
					 move(y-1,maxX-1);
					 strPos--;
				 }
			}else if(key == KEY_RIGHT){
				 if(strPos<bLen && x<maxX-1){
					 move(y,x+1);
					 strPos++;
				 }else if (strPos <bLen && x>=maxX-1){
					 move(y+1,0);
					 strPos++;
				 }
				 
			}else if (key == KEY_ENTER || key == 10 || key == 13){
				*(buffer+bLen) = '\0';
				move(origY,origX);
				strPos = bLen;
				moveRight(maxX, 0,bLen);
				addch('\n');
				if(bLen>0){
					add_elem(strdup(buffer),&head);
					histLen++;
				}
				break;
			}else if (key < 127 && key >31) {
				int count = 0;
				int yOffset;

				bLen++;
				insertChar(buffer,key,strPos,bLen);
				buffer[bLen] = '\0';
					yOffset= (pLen%maxX+bLen-1)/maxX;
					if(yOffset>0&& yOffset+origY > maxY-1){
						origY = (maxY-1) - yOffset;
					}
					move(origY, origX);
					yOffset= (pLen%maxX+bLen)/maxX;
					if(yOffset>0&& yOffset+origY > maxY-1){
						origY = (maxY-1) - yOffset;
					}
					addstr(buffer);
					move(origY, origX);					
				while(count<=strPos){
					getyx(stdscr,y,x);
					if(x<maxX-1){
						move(y,x+1);
						count++;
					}else {
						if(y<maxY-1){
							move(y+1,0);
						}else {	
							addch('\n');
						}
						count++;
					}
				}
				strPos++;
			}
			if(key!=23){ /*if we didn't consecutively cut words, we need to clear */
				flagUW = 1;
			}
		}
		refresh();
		/* dollar sign*/
		start = my_strrfind(buffer,'$');
		if(start!=NULL){
			int dollarIndex = my_strindex(buffer, '$');
			int startIndex = my_strindex(buffer, '(')+1;
			int endIndex = (my_strindex(buffer, ')')-1);
			int sizeToCopy = (endIndex - startIndex)+1;
			char* param  = (char*)malloc(sizeof(sizeToCopy+1));
			char* funct = (char*)malloc(sizeof(startIndex+1));
			char* retVal;
			my_strncpy(param,&(buffer[startIndex]),sizeToCopy);
			param[sizeToCopy] = '\0';
			my_strncpy(funct,buffer,dollarIndex);
			funct[dollarIndex] = '\0';
			retVal= execute(2, my_str2vect(param));
			free(param);
			tmpBuffer = buffer;
			buffer = my_strconcat(funct,retVal);
		}
			
		
		if(my_strcmp(buffer, "help")==0 ){
			help();
		}else if(my_strcmp(buffer, "exit")==0){
			  free(buffer);
			exitShell();
		}else if(my_strncmp(buffer, "cd ", 3)==0){
			if(cd(&(buffer[3]))==-1){
				jerror(" cannot cd into ", &(buffer[3]));
			}
		}else if(my_strncmp(buffer, "cd", 2)==0 && my_strlen(buffer)==2){
			jerror(" cannot cd into nothing. Need to specify path","");
		}else if(my_strncmp(buffer, "./", 2)==0){
			execute(1,my_str2vect(buffer));
		}else{
			execute(1,my_str2vect(buffer));
		}
		if(tmpBuffer !=NULL){
			free(buffer);
			buffer = tmpBuffer;
			tmpBuffer = NULL;
		}
		refresh();
		buffer[0]= '\0';
	} 
    return 1;
}
