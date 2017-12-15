#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include<time.h>
HANDLE hEvent1, hEvent2, hEvent3, hEvent4;
int a[5];
HANDLE hThr;
unsigned long uThrID;
int full;
int data;
FILE *f;

int produce()
{
	Sleep(500);
	return rand() % 200;
}
void writeToFile(int cons, int processing, bool prod);

void *producer(void *)
{
	while (1)
	{
		int t = produce();
		WaitForSingleObject(hEvent1, INFINITE);
		while (full)
		{
			SetEvent(hEvent1);
			WaitForSingleObject(hEvent2, INFINITE);
		}
		data = t;
		full = 1;

		writeToFile(t, 0, true);
		//printf("Work(Producer)\n");
		SetEvent(hEvent3);
		SetEvent(hEvent1);

	}
	return NULL;
}

void *consumer(void *)
{
	while (1)
	{
		int t;
		WaitForSingleObject(hEvent4, INFINITE);
		while (!full)
		{
			//printf("Wait producer(Consumer)\n");
			WaitForSingleObject(hEvent3, INFINITE);
		}
		t = data;
		writeToFile(t, t * t * t, false);
		full = 0;

		//printf("Work(Consumer)\n");
		SetEvent(hEvent2);
		SetEvent(hEvent4);

	}
	return NULL;
}

int main(void)
{
	srand(time(0));
	f = fopen("out.txt", "w");	
	hEvent1 = CreateEvent(NULL, FALSE, TRUE, NULL);
	hEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent3 = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent4 = CreateEvent(NULL, FALSE, TRUE, NULL);
	printf("Enter any char to close a program\n");
	hThr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)producer, NULL, 0, &uThrID);
	hThr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)consumer, NULL, 0, &uThrID);

	getch();
	fclose(f);
	return 0;
}



void writeToFile(int cons, int processing, bool prod)
{

	if (prod)
	{
		printf("produced %d\n", cons);
		fprintf(f, "produced %d\n", cons);
	}
	else
	{
		fprintf(f, "consumed %d\t\tprocessed %d\n", cons, processing);
		printf("consumed %d\t\tprocessed %d\n", cons, processing);
	}
}
