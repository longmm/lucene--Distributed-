#ifndef MY_STORE
#define MY_STORE

#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

#define DEF_BUF_SIZE 1024;

using namespace std;

namespace store{

class IndexInput{
	public:
	    virtual long getFilePointer()=0;
        virtual long length()=0;
		virtual int readChar(char* c)=0;
		virtual int  readChars(char* c,int len)=0;
		virtual long seek(long pos)=0;
		bool skipChars(int length);
		int  readWchars(wchar_t* c,int len);
		int readWString(wstring*);
		int  readInt(int*);
	    int readLong(long*);
	    int readFloat(float*);
};
class IndexOutput{
       public:
		   virtual bool close()=0;
	       virtual bool flush()=0;
		   virtual long getFilePointer()=0;
		   virtual long length()=0;
		   virtual long seek(long pos)=0;
		   virtual int writeChar(char c)=0;
		   virtual int writeChars(char* c,int length)=0;
		   int writeWchars(wchar_t* wc,int len);
		   void writeWString(wstring wc);
		   int writeInt(int i);
           int writeLong(long i);
           int writeFloat(float f);
};
class BufferedIndexInput:public IndexInput{
    private:
		char* buffer;
	    long bufferStart;
		int bufferLength;
		int bufferPosition;
		int bufferSize;

	public:
		int BUFFER_SIZE;
	     BufferedIndexInput();
	     BufferedIndexInput(int bufferSize);
		 ~BufferedIndexInput();
		 int getBufferSize();
		 long getFilePointer();
		 int readChar(char* c);
		 int readChars(char* c,int len);
		 virtual int readInternal(char* c,int len)=0;
		 int refill();
		 virtual long seekInternal(long pos)=0;
		// bool setBufferSize(int newSize);
		 long seek(long pos);
};
class BufferedIndexOutput:public IndexOutput{
     private:
		 int BUFFER_SIZE;
	     char* buffer;
		 long bufferStart;
		 int bufferPosition;
	 public:
		  BufferedIndexOutput();
          BufferedIndexOutput(int buffersize);
		 ~BufferedIndexOutput();
          bool close();
		  bool flush();
		  virtual int flushBuffer(char* c,int len)=0;
		  long getFilePointer();
		  int writeChar(char c);
		  int writeChars(char* c,int len);
		  long seek(long pos);
		  virtual long seekInternal(long pos)=0;
		  void init();
};
class FSRDescriptor{
    public:
	bool isOpen;
	long position;
	long length;
	int fd;
	char* filename;
	FSRDescriptor(char* filename);
    ~FSRDescriptor();
    long seek(long pos);
    int readChars(char* c,int len);
	long getLength();
};
class FSIndexInput:public BufferedIndexInput
{
      private:
         FSRDescriptor* fsfile;
	     pthread_mutex_t mylock;
	  public:
		  FSIndexInput(char* filepath);
          FSIndexInput(char* filepath,int);
		  ~FSIndexInput();
		  long length();
		  int readInternal(char* c, int len);
          long seekInternal(long pos);
};
class FSWDescriptor
{
    public:
	bool isOpen;
	long position;
	long length;
	int fd;
	char* filename;
    FSWDescriptor(char* filename);
	~FSWDescriptor();
    int seek(long pos);
    int writeChars(char* c,int len);
	long getLength();
};
class FSIndexOutput:public BufferedIndexOutput
{
     private:
		 FSWDescriptor* fswfile;
	 public:
		 FSIndexOutput(char* filepath);
	     FSIndexOutput(char* filepath,int bufsize);
	     ~FSIndexOutput();
		 long length();
		 long seekInternal(long pos);
         int flushBuffer(char* c, int len);
};
class FSLock
{
    private:
        wstring lockfile;
	    wstring lockdir;
		char* filename;
		bool isLock;
	public:
		FSLock(wstring lockfile,wstring lockdir);
	    ~FSLock();
		bool isLocked();
		bool obtain();
		bool release();
};
union F2I{
   float f;
   char cs[4];
};
};

#endif
