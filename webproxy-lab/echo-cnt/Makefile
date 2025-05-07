CC = gcc                 # 사용할 컴파일러는 gcc
CFLAGS = -Wall -O2       # 컴파일 시 모든 경고 출력(-Wall) + 최적화(-O2) 옵션 사용
OBJS = csapp.o echo-cnt.o    # 공통으로 사용할 객체 파일 목록 (서버와 클라이언트 모두 사용함)

all: echoservert-pre echoclient  # make만 입력했을 때 기본으로 빌드할 대상

sbuf.o: sbuf.c csapp.h
	$(CC) $(CFLAGS) -c sbuf.c -pthread

echo-cnt.o: echo-cnt.c csapp.h
	$(CC) $(CFLAGS) -c echo-cnt.c -pthread

echoservert-pre: sbuf.o echoservert-pre.o $(OBJS)  # echoserveri를 만들기 위해 필요한 파일들
	$(CC) $(CFLAGS) -o echoservert-pre echoservert-pre.o sbuf.o echo-cnt.o csapp.o -pthread

echoclient: echoclient.o $(OBJS)    # echoclient를 만들기 위해 필요한 파일들
	$(CC) $(CFLAGS) -o echoclient echoclient.o echo-cnt.o csapp.o -pthread

clean:                 # make clean 입력 시 실행됨 (빌드된 파일들을 삭제)
	rm -f *.o echoservert-pre echoclient  # 객체 파일과 실행파일들을 모두 삭제