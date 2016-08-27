
#include <iostream>
#include <fstream>
#include <iterator>
#include <cstdlib>
#include <unordered_map>
#include <queue>
#include <time.h>


struct counters
{
    int startWin;
    int startLB;
    int endLB;
    int skip;
};

struct params
{
    unsigned char N;
    unsigned char L;
    unsigned char S;
    int W;
    int F;
    int maxLit;
};

struct buffer
{
    int totalSize;
    int bitsUsed;
    unsigned char arr[500];
};


typedef std::unordered_map<std::string, std::queue<int> > Dict;

int set_param(const char * s, unsigned char * var, unsigned char param, int low, int high);
void outputParams(struct params * p);

void addString(Dict * overallDict, std::string s, int offset);
int matchLength(char window[], Dict * overallDict,struct counters * count, struct params * p, bool clearLiteral);
void addToDict(int charsToAdd,char window[], Dict * overallDict, struct counters * count, struct params * p);
void removeFromDict(Dict * overallDict, char buffer[], int n, char window[], struct counters * count,struct params * p);
void iniWindow(struct params *p,char window[],struct counters * count, char * fileName);
std::string getChars(int n, const char *fileName);
void encode2(struct params *p, const char *fileName);

void printArr(char * arr, int len);
void printDict(Dict * dict);
void fullPrint(char window[], int length,struct counters * c);


int main(int argc, const char * argv[])
{
    unsigned char N = 11, L = 4, S = 3, param_val;
    int i, count;
    
    for (i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            char param[100];
            count = 3;
            while (argv[i][count] != '\0')
            {
                param[count - 3] = argv[i][count];
                count++;
            }
            
            param[count - 3] = '\0';
            param_val = atoi(param);
            
            switch (argv[i][1])
            {
                case 'N':
                    if (set_param("N", &N, param_val, 9, 14)) return 1;
                    break;
                case 'L':
                    if (set_param("L", &L, param_val, 3, 4)) return 1;
                    break;
                case 'S':
                    if (set_param("S", &S, param_val, 1, 5)) return 1;
                    break;
            }
        }
    }
    
    struct params p;
    p.N = N;
    p.L = L;
    p.S = S;
    p.F = (1 << L) - 1;
    p.W = 1 << N;
    p.maxLit = (1 << S) - 1;
    
    encode2(&p, argv[argc - 1]);
    
    return 0;
}


int set_param(const char * s, unsigned char * var, unsigned char param, int low, int high)
{
    if (param < low || param > high)
    {
        std::cerr << "Value for " << s << " out of range.\n";
        return 1;
    }
    *var = param;
    return 0;
}


void outputParams(struct params * p)
{
    std::cout << p->N << p->L << p->S;
    std::cerr << "Value for N: " << p->N << "\n";
    std::cerr << "Value for L: " << p->L << "\n";
    std::cerr << "Value for S: " << p->S << "\n";
}


void addString(Dict * overallDict, std::string s, int offset)
{
    Dict::iterator one = overallDict->find(s);
    
    if (one == overallDict->end())  // then s is not in the dictionary
    {
        std::queue<int> newQueue;
        newQueue.push(offset);
        
        std::pair<std::string, std::queue<int> > newPair(s, newQueue);
        overallDict->insert(newPair);
    }
    else
        (one->second).push(offset);
}


void addToDict(int charsToAdd, char window[], Dict * overallDict, struct counters * count, struct params * p)
{
    int i, j;

    // For each character that slides to the left of the LB
    for (i = 0; i < charsToAdd; i++)
    {
        // Add each string up to F chars to its right.
        std::string s = "";

        for (j = 0; j < std::min(p->F, count->endLB - count->startLB + charsToAdd); j++) // Go only upto how many chars left in LB
        {
            s += window[(count->startLB - i - 1 + j) % p->W];

            addString(overallDict, s, (count->startLB - i - 1) % p->W); // The offset here is probably wrong
        }
    }
}

void outputEncoding(int bitsAdding, int matchLength, bool clear)
{
    static unsigned char buff = 0;
    static int position = 0;
    
    unsigned char c = 0, mask, saved;
        

    if (clear){
        std::cout << buff;
        return;
    }


    if (matchLength % 2 == 1)
        bitsAdding = 4;
    else
        bitsAdding = 8;
    
    if (matchLength >= 2)
    {
        c = (char)matchLength;
        
        if (8 - position - bitsAdding >= 0)
        {
            c <<= 8 - bitsAdding - position;
            position += bitsAdding;
            
            buff |= c;
            
            if (position == 8)
            {
                std::cout << buff;
                position = 0;
                buff = 0;
            }
        }
        else
        {
            mask = (1 << (bitsAdding - (8 - position))) - 1;
            saved = c & mask;
            
            c >>= bitsAdding - (8 - position);
            
            buff |= c;
            
            std::cout << buff;
            buff = 0;
            
            saved <<= 16 - position - bitsAdding;
            
            buff |= saved;
            
            position = bitsAdding - (8 - position);
            
        }
    }
}


int matchLength(char window[], Dict * overallDict, struct counters * count, struct params * p, bool clearLiteral)
{
    int matchLength = 1, i, offset = -1, newOffset, mask;
    std::string st, s = "";

    
    static char literalBuffer[32];
    static int buffPos = 0;

    if (clearLiteral){
        outputEncoding(p->L, 0,0);
        outputEncoding(p->S, buffPos,0);
        for (int k =0; k<buffPos ;k++)
                std::cout << literalBuffer[k];
    }

    for (i = 0; i < std::min(p->F, count->endLB - count->startLB); i++)
    {
        s += window[(count->startLB + i) % p->W];
        
        Dict::iterator f = overallDict->find(s);
        
        if (f != overallDict->end())
        {
            st = s;
            matchLength = i + 1;
            offset = (f->second).front();
        }
        else
            break;
    }
    
    if (matchLength >= 2)
    {
        if (buffPos >0){
            outputEncoding(p->L, 0,0);
            outputEncoding(p->S, buffPos,0);
            for (int c =0; c<buffPos;c++)
                std::cout << literalBuffer[c];
            buffPos = 0;
        }

        outputEncoding(p->L, matchLength - 1,0);
        
        newOffset = offset >> (p->N - 8);
        outputEncoding(8, newOffset,0);
        
        mask = (1 << (p->N - 8)) - 1;
        offset &= mask;
        outputEncoding(p->N - 8, offset,0);


    }
    else
    {
        literalBuffer[buffPos++] = window[(count->startLB)];
        if (buffPos == p->maxLit){
            outputEncoding(p->L, 0,0);
            outputEncoding(p->S, p->maxLit,0);
            for (int c =0; c<p->maxLit;c++)
                std::cout << literalBuffer[c];
             
            buffPos =0;
        }
    }



        



    return matchLength;
}


void printDict(Dict * dict)
{
    Dict::iterator it = dict->begin();
    
    for (; it != dict->end(); it++)
        std::cout << "\n" << it->first << std::endl;
}


std::string getChars(int n, const char * fileName)
{
    std::string s = "";
    int i;
    static std::ifstream input(fileName);
    static std::istream_iterator<char> eos;
    static std::istream_iterator<char> iit(input);

    for (i = 0; i < n; i++)
        if (iit != eos)
            s +=  *(iit++);

    return s;
}


void iniWindow(struct params * p, char window[], struct counters * count, const char * fileName)
{
    //Get F chars and put em in window.
    std::string s = getChars(p->F, fileName);
    int i;
    
    for (i = 0; i < s.size(); i++)
        window[i] = s.at(i);

    count->endLB = i;
}


void removeFromDict(Dict * overallDict, char buffer[], int lengthOfMatch, char window[], struct counters * count, struct params * p)
{
    int i, j;
    
    for (i = 0; i < lengthOfMatch; i++)
    {
        std::string s = "";
        
        
        for (j = i; j < std::min(p->F + i, count->endLB - (count->startWin - lengthOfMatch + i)); j++)
        {
            if (j < lengthOfMatch)
                s += buffer[j];  // first we add chars from the buffer where we saved the overwritten chars from the window
            else
                s += window[(count->startWin + (j - lengthOfMatch)) % p->W];
            
            Dict::iterator f = overallDict->find(s);
            
//            std::cout << "removed string: " << s << std::endl;
//            
//            if (f == overallDict->end())
//                std::cout << "key not in!" << std::endl;
//            
//            std::cout << "queue size: " << (f->second).size() <<  std::endl;
            
            (f->second).pop();  // remove the oldest offset value from the queue corresponding to s
            
            if ((f->second).size() == 0)  // if the queue is now empty, remove the key from the dictionary
                overallDict->erase(s);
        }
    }
}


void encode2(struct params * p, const char *fileName)
{
    int W = p->W, F = p->F, N = p->N, c = 0, c2 = 0, bufferLength = 0;
//    p->F = 7;
//    p->W = 16;
    outputParams(p);   
    int total = 0;
    double loopSum = 0, addSum = 0, removeSum = 0;
    
    clock_t t, whole;
    
    std::ifstream input(fileName);
    std::string s;
    char window[p->W], buffer[p->F];  // size of window needs to be changed back to W once testing is done
    
    struct counters count;
    count.startWin = 0; // index of the first char of the window
    count.startLB = 0; // index of the first char of the lookahead buffer
    count.endLB = 0;  // index of the last char in the lookahead buffer
    count.skip = 0;

    Dict overallDict(245760);

    //Gets F characters and puts them into the window. Sets endLB to F.
    iniWindow(p, window, &count, fileName);
    
    bool endOfFile = 0;
    int lengthOfMatch, readyToRemove = 0, splitString = 0;
    
    int numMatched = 0;
    
    
    while (count.endLB - count.startLB != 0 || count.startLB == 0) //endLB - startLB == 0 at start hence the or.
    {
        total++;
        whole = clock();
//        fullPrint(window, 16, &count);

        // Just finds the longest length match.
        lengthOfMatch = matchLength(window, &overallDict, &count, p,0);

        //Increment the counters appropriately based on length of match.
        if (count.startLB - count.startWin ==  W - F)
        {
            count.startWin += lengthOfMatch;
            
            if (readyToRemove == 0)
                readyToRemove = 1;
        }
        
        count.startLB += lengthOfMatch;
        
//        std::cout << "len of match: " << lengthOfMatch << std::endl;
        
        numMatched += lengthOfMatch;
//        std::cout << "matched so far: " << numMatched << std::endl;
        
//        std::cout << "startWin: " << count.startWin % p->W << "  startLB: " << count.startLB % p->W << "  endLB: " << count.endLB % p->W << std::endl;
        
        if (count.startLB - count.startWin > W - F)
        {
            count.startWin = (count.startLB - (W - F));
            readyToRemove = 1;
        }

        // Since we shifted lengthOfMatch characters out of the LB and into the window, fill in
        // lengthOfMatch characters to end of LB.
        
        // Only do this if there are still characters in the file remaining.

        // Probably want to first put the characters you're writing over into a buffer
        // so that removeFromDict knows which entries to delete.
        
        if (!endOfFile)
        {
            s = getChars(lengthOfMatch, fileName);
            
            if (s == "")
                endOfFile = 1; // if there are no more characters. I.e, its the end of the file
            else
            {
                for (bufferLength = 0, c = 0; c < s.size(); c++)
                {
                    if (count.endLB + c >= p->W)  // this condition ensures we don't use the buffer unless we need to
                        buffer[bufferLength++] = window[(count.endLB + c) % p->W];
                    
                    window[(count.endLB + c) % p->W] = s[c];
                }
                
                // this will happen when the length of the match is longer than the number of chars left in inputStream
                while (c < lengthOfMatch)
                {
                    buffer[bufferLength++] = window[(count.endLB + c) % p->W];
                    c++;
                    splitString = 1;  // we set this to 1 instead of endOfFile so we can ensure we adjust endLB appropriately
                }
            }
        }
        
        // if there are no more chars to read in from the file, we have to create the buffer from the start of the window where
        //  we are overwriting stuff.
        if (endOfFile)
        {
            for (bufferLength = 0, c2 = count.startWin - lengthOfMatch; c2 < count.startWin; bufferLength++, c2++)
                buffer[bufferLength] = window[c2 % p->W];
        }
        
        // Adds the new entries into the dictionary
        //  this has to go after adding the new characters to the window so we can add those to the dictionary if necessary
        t = clock();
        addToDict(lengthOfMatch, window, &overallDict, &count, p);
        t = clock() - t;
        addSum += t;
        
        
        if (!endOfFile)
            count.endLB += s.size();
        
        if (splitString)
            endOfFile = 1;
        
        // at this point in time, startWin is the index including the start of the window, startLB is the index including the start of the lookahead buffer and endLB is the index not including the end of the lookagead buffer i.e. if endLB is 8, then the last valid index of the lookahead buffer is 7
        
        if (readyToRemove)
        {
            t = clock();
            removeFromDict(&overallDict, buffer, bufferLength, window, &count, p);
            t = clock() - t;
            removeSum += t;
        }
        
        whole = clock() - whole;
        loopSum += whole;
    }
    
    outputEncoding(0,0,1);
    matchLength(window, &overallDict, &count, p,1);
   // std::cout << "loop avg: " << loopSum / total << std::endl;
    //std::cout << "add avg: " << addSum / total << std::endl;
    //std::cout << "remove avg: " << removeSum / total << std::endl;
}




void fullPrint(char window[], int length, struct counters * c)
{
    std::cout << "\n";
    int count;
    
    for (count = 0; count < std::min(16, c->endLB); count++)
    {
        if (count == (*c).startLB % 16  )
            std::cout << " | ";
        
        if (count == c->startWin % 16) {
            std::cout << " * ";
        }
        
        std::cout << window[count];
    }
    std::cout << std::endl;
    
    for (count = 0; count < std::min(16, c->endLB); count++)
        std::cout << count;
    std::cout << std::endl;
}


void printArr(char * arr, int len)
{
    int c;
    for (c = 0; c < len; c++)
        std::cout << arr[c];
    std::cout << std::endl;
}



