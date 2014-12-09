/*********************************************************
 *				Database - Final Project				 *
 *					1114606 ������						 *
 *				����Ȱ��-������ ���� ���α׷�				 *
 *					High-Level Lanuage					 *
 *********************************************************/

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#pragma comment(lib, "libmysql.lib")

const char *host = "localhost";
const char *user = "sj7226";
const char *pw = "1234";
const char *db = "library_information_system";

int loginstate = 0;

void showPerson (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row);
void showWork (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row);
void showRecruiting (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row);
void searchResult (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, int n);
void searchResult_work (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, int n);

int checkUser (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char* id, char* pw);

char* searchTitleByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]);
char* searchAuthorsByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]);
char* searchPublisherByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]);
bool searchAvailabilityByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]);
void insertBookInfo ();
void editBookInfo ();
void deleteBookInfo ();

int main(int argc, CHAR* argv[])
{
	MYSQL *connection = NULL;
	MYSQL conn;
	MYSQL_RES *sql_result = NULL;
    MYSQL_ROW sql_row = NULL;

	int signup_state;

	char id[10];
	char password[10];
	char sname[30];
	char department[30];

	char contact[30];
	char query[200];
	int wno = NULL;
	char wcontent[40] = {NULL};
	int dotime = NULL;
	char date[15];
	char cycle[10];
	char state[10];
	int i = 0;
	int j = 9;

	int n = 0;
	int d = 0;
	int w = 0;
	int rs = 0;
	int select_num;

	if(mysql_init(&conn) == NULL)
		printf("mysql_init() error!");
		
	if(mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0))
	{
		while(true)
		{	printf("<< Library Information System >>\n\n");
			printf("  1. Sign In\n");
			printf("  2. Sign Up\n");
			scanf("%d",&i);

			if(i == 1) {
				//����� �����Ͱ� user_tb�� �ִ��� Ȯ��
				//printf("connection is good!\n");
				printf("<< Sign In >>\n");
				printf("your ID : ");
				scanf("%s", id);
				printf("your password : ");
				scanf("%s", password);

				//sprintf(query,"insert into Volunteer(vno, vname, job, contact) values('%d', '%s', '%s', '%s')", vno, vname, job, contact);
				signup_state = checkUser(&conn, sql_result, sql_row, id, password);
				//sprintf(query,"SELECT * FROM user_tb WHERE ID='%s' AND password='%s'", id, password);
				//login_state = mysql_query (&conn, query);

				if(signup_state == 0) {
					if(loginstate == 1) {
						printf("You Signed in! Welcome to LIS!\n");
					} else if (loginstate == 0) {
						printf("Sign up failed! Try again.\n");
					}
				} else {
					printf("No Connection to Database! \n");
				}

			} else if( i == 2) {
				printf("<< Sign Up >>\n");
				printf("ID (7 digits) : ");
				scanf("%s", &id);
				printf("password (under 8 characters) : ");
				scanf("%s", &password);
				printf("name : ");
				fflush(stdin);
				gets(sname);
				printf("department : ");
				scanf("%s", department);
				
				sprintf(query,"insert into user_tb(ID, password, name, department) values('%s', '%s', '%s', '%s')", id, password, sname, department);
				mysql_query (&conn, query);
				printf("You Signed up! Welcome to LIS!\n");
			}
				/* �� �κп� ����Ÿ���� �缭���� �л����� �Ǻ��ϴ� ����
			
			if (�л��̸�){

			] else if (�缭�̸�){*/
				
				while (true){
					printf("<< Library Information System >>\n\n");
					printf("  1. Register New Book\n");
					printf("  2. Edit Book Information\n");
					printf("  3. Delete Book Information\n  >>");
					printf("  0. Back\n\n  >>");
					scanf("%d",&select_num);

					if (select_num == 1){ //å�߰�
						insertBookInfo();
				
					} else if (select_num == 2){  //å ���� ����
						editBookInfo();
						
					} else if (select_num == 3){ //å ����
						deleteBookInfo();

					} else if (select_num == 0){
						break;

					}else 
						printf("Please reenter number which is correct.");
				}
			/*if(i==2) {
				//volunteer ���̺� ����Լ� - ������ ��ȸ
				showPerson(&conn, sql_result, sql_row);
			}

			if(i==3) {
				//Work���̺� �����͸� �����Ѵ�.
				printf("����Ȱ�� ��ȣ : ");
				scanf("%d", &wno);
				printf("Ȱ������ : \n");
				fflush(stdin);
				gets(wcontent);
				printf("\n");
				printf("Ȱ���ð�(4) : ");
				scanf("%d", &dotime);
				printf("��¥(yyyy-mm-dd) : ");
				scanf("%s", date);
				printf("�ֱ�(Regular �Ǵ� Irregular) : ");
				scanf("%s", cycle);
				sprintf(query,"insert into work(wno, wcontent, dotime, date, cycle) values('%d', '%s', '%d', '%s', '%s')", wno, wcontent, dotime, date, cycle);
				mysql_query (&conn, query);
				printf("��ϵǾ����ϴ�.\n");

				//work���̺� ����Լ�
				showWork(&conn, sql_result, sql_row);
			}

			if(i==4) {
				//work���̺� ����Լ� - ����Ȱ�� ��ȸ
				showWork(&conn, sql_result, sql_row);
			}

			if(i==5) {
				//Ư�� �����ڸ� Ư�� ����Ȱ���� ��Ī��Ų��(����Ȱ���� ��û�Ѵ�.)
				printf("�����ڿ��� : ");
				scanf("%d", &vno);
				printf("����Ȱ�� ��ȣ : ");
				scanf("%d", &wno);
				printf("Ȱ������ : ");
				scanf("%s", state);
				sprintf(query,"insert into Recruiting(vno, wno, state) values('%d', '%d', '%s')", vno, wno, state);
				mysql_query (&conn, query);
				printf("��ϵǾ����ϴ�.\n");
			
				showRecruiting(&conn, sql_result, sql_row);
			}
			if(i==6) {
				//����Ȱ�� ������ ��ȸ�Ѵ�.
				printf("������ ��ȸ�� �������� ������ �Է��ϼ���. : ");
				scanf("%d", &n);
				searchResult(&conn, sql_result, sql_row, n);
				//searchResult_work(&conn, sql_result, sql_row, n);
			}
			if(i==7) {
				//���系���� ��ȸ�Ѵ�.
				printf("���系���� ��ȸ�� �������� ������ �Է��ϼ���. : ");
				scanf("%d", &n);
				//searchResult(&conn, sql_result, sql_row, n);
				searchResult_work(&conn, sql_result, sql_row, n);
			}

			if(i==8) {
				//����ڰ� ������ �����͸� �����ϵ��� �Է� �޴´�.
				printf("� �����͸� �����Ͻðڽ��ϱ�? (1:������  2:����Ȱ��  3:��û����)\n");
				scanf("%d", &d);
			
				if(d == 1) {
					//������ ������ ����
					printf("�������� ������ ���� �Է� : \n");
					scanf("%d", &n);
					sprintf(query,"delete from volunteer where vno='%d'", n);
					mysql_query (&conn, query);
					printf("�����Ǿ����ϴ�.\n");
					showPerson(&conn, sql_result, sql_row);
				}

				if(d == 2) {
					//����Ȱ�� ������ ����
					printf("�������� ����Ȱ�� ��ȣ �Է� : \n");
					scanf("%d", &w);
					sprintf(query,"delete from work where wno='%d'", w);
					mysql_query (&conn, query);
					printf("�����Ǿ����ϴ�.\n");
					showWork(&conn, sql_result, sql_row);
				}

				if(d == 3) {
					//��û���� ������ ����
					printf("��û�� öȸ�� �������� ������ ����Ȱ�� ��ȣ �Է� : \n");
					printf("�����ڿ��� : ");
					fflush(stdin);
					scanf("%d", &n);
					printf("����Ȱ�� ��ȣ : ");
					fflush(stdin);
					scanf("%d", &w);
					sprintf(query,"delete from recruiting where vno='%d' and wno='&d'", n, w);
					mysql_query (&conn, query);
					printf("�����Ǿ����ϴ�.\n");
					showRecruiting(&conn, sql_result, sql_row);
				}
			}*/
		}	
	}
	mysql_free_result(sql_result);
	mysql_close(connection);
	return 0;
}

void insertBookInfo (){
	MYSQL *connection = NULL;
	MYSQL conn;
	MYSQL_RES *sql_result = NULL;
    MYSQL_ROW sql_row = NULL;
	char title[30]; 
	char authors[10];
	char publisher[20];
	char isbn[18];
	char query[200];

	printf("<< Register New Book >>\n\n");
	printf("Title : ");
	scanf("%s", title);
	printf("Authors : ");
	scanf("%s", authors);
	printf("Publisher : ");
	scanf("%s", publisher);
	printf("ISBN : ");
	scanf("%s", isbn);
	fflush(stdin);
	sprintf(query,"insert into book_tb(title, authors, publisher, ISBN) values('%s', '%s', '%s', '%s')", title, authors, publisher, isbn);
	mysql_query (&conn, query);
	printf("You successfully completed registration of the new book!\n\n");

	mysql_free_result(sql_result);
	mysql_close(connection);
}

void editBookInfo (){
	MYSQL *connection = NULL;
	MYSQL conn;
	MYSQL_RES *sql_result = NULL;
    MYSQL_ROW sql_row = NULL;

	int select_edit;
	int select_avail;
	int id[7];
	
	char title[30]; 
	char authors[10];
	char publisher[20];
	char isbn[18];
	char newisbn[18];
	char query[200];

	bool availability;
	
	char* bookname;
	char* authors_p;
	char* publisher_p;
	char* isbn_p;

	while(1){
		printf("<< Edit Book Information >>\n\n");
		printf("Please enter ISBN of the book which you want to edit information.\n");
		printf("ISBN : ");
		fflush(stdin);
		scanf("%s", isbn);
		bookname = searchTitleByISBN (&conn, sql_result, sql_row, isbn);
		if(bookname == " ")
			printf("The book which you search by ISBN doesn't exist in this system.");
		else
			break;
	}
	while(1){
		printf("The book name which you selected is %s.\n", bookname);
		printf("What do you want to edit the book's information?\n");
		printf("1. Title\n");
		printf("2. Authors\n");
		printf("3. Publisher\n");
		printf("4. ISBN\n");
		printf("5. Availability\n");
		printf("0. Back\n");
		fflush(stdin);
		scanf("%d", &select_edit);
		if(select_edit == 1){ //å title ����
			printf("This book's title is %s. Enter new title of the book. \n>> ", bookname);
			scanf("%s", title);
			sprintf(query,"update book_tb set title = '%s' where ISBN = '%s'", title, isbn);
			mysql_query (&conn, query);
			printf("You successfully changed information of the book!\n\n");

		} else if (select_edit == 2){  //å ���� ����
			authors_p = searchAuthorsByISBN (&conn, sql_result, sql_row, isbn);
			printf("This book's authors are %s. Enter new authors of the book.\n>> ", authors_p);
			scanf("%s", authors);
			sprintf(query,"update book_tb set authors = '%s' where ISBN = '%s'", authors, isbn);
			mysql_query (&conn, query);
			printf("You successfully changed information of the book!\n\n");

		} else if (select_edit == 3){  //å ���ǻ� ����
			publisher_p = searchPublisherByISBN (&conn, sql_result, sql_row, isbn);
			printf("This book's publisher is %s. Enter new publisher of the book.\n>> ", publisher_p);
			scanf("%s", publisher);
			sprintf(query,"update book_tb set publisher = '%s' where ISBN = '%s'", publisher, isbn);
			mysql_query (&conn, query);
			printf("You successfully changed information of the book!\n\n");

		} else if (select_edit == 4){  //å ISBN ����
			printf("This book's ISBN is %s. Enter new ISBN of the book.\n>> ", isbn);
			scanf("%s", newisbn);
			sprintf(query,"update book_tb set ISBN = '%s' where ISBN = '%s'", newisbn, isbn);
			mysql_query (&conn, query);
			printf("You successfully changed information of the book!\n\n");
		
		} else if (select_edit == 5){  //å �뿩���� ����
			availability = searchAvailabilityByISBN (&conn, sql_result, sql_row, isbn);
			if(availability == true){
				while(1){
					printf("The book can be rented. Do you want to change it?\n");
					printf("1. Yes\n");
					printf("2. No\n>> ");
					scanf("%d",&select_avail);
					if(select_avail == 1){
						printf("Enter ID of the renting student.\n");
						scanf("%s", id);
						sprintf(query,"update book_tb set availavility = 'false', renting_student = '%d' where ISBN = '%s'", id, isbn);
						mysql_query (&conn, query);
						printf("You successfully changed information of the book!\n\n");
						break;
					} else if (select_avail == 2){
						break;
					} else
						printf("Please reenter number which is correct.");
				}
			} else if (availability == false){
				while(1){
					printf("The book have been rented. Do you want to change it?\n");
					printf("1. Yes\n");
					printf("2. No\n>> ");
					scanf("%d",&select_avail);
					if(select_avail == 1){
						sprintf(query,"update book_tb set availavility = 'true', renting_student = null where ISBN = '%s'", isbn);
						mysql_query (&conn, query);
						printf("You successfully changed information of the book!\n\n");
						break;
					} else if (select_avail == 2){
						break;
					} else
						printf("Please reenter number which is correct.");
				}
			}

		} else if (select_edit == 0){
			mysql_free_result(sql_result);
			mysql_close(connection);
			break;
		
		} else
			printf("Please reenter number which is correct.");
	}
}

void deleteBookInfo (){
	MYSQL *connection = NULL;
	MYSQL conn;
	MYSQL_RES *sql_result = NULL;
    MYSQL_ROW sql_row = NULL;
	
	char isbn[18];
	char* bookname;
	char query[200];
	int select_delete;
	int n = 0;
	int d = 0;
	int w = 0;
	int rs = 0;

	while(1){
		printf("<< Delete Book Information >>\\nn");
		printf("Please enter ISBN of the book which you want to delete.\n");
		printf("ISBN : ");
		fflush(stdin);
		scanf("%s", isbn);
		bookname = searchTitleByISBN (&conn, sql_result, sql_row, isbn);

		if(bookname == " ")
			printf("The book which you search by ISBN doesn't exist in this system.");
		
		else
			break;
	}

	while (1){
		printf("The book name which you selected is %s.\n",bookname);
		printf("Do you want to delete this book?\n");
		printf("1. Yes\n");
		printf("2. No\n");
		fflush(stdin);
		scanf("%d", &select_delete);

		if (select_delete == 1){
			sprintf(query,"delete from book_tb where ISBN = %s", isbn);
			mysql_query (&conn, query);
			printf("You successfully deleted information of the book!\n\n");
			showRecruiting(&conn, sql_result, sql_row);
			break;

		}else if (select_delete == 2)
			break;

		else
			printf("Please reenter number which is correct.");
	}

	mysql_free_result(sql_result);
	mysql_close(connection);
}
void showPerson (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row) {
	char *query = "select *from volunteer order by vno";
	int state = 0;

	state = mysql_query(connection, query);
	if(state == 0)
	{
		printf("+-----------------------------------------------+\n");
		printf("+  Vno  |    name    |   job   |     contact    +\n");
		printf("+-----------------------------------------------+\n");
		sql_result = mysql_store_result(connection);			// Result Set�� ����
		while((sql_row = mysql_fetch_row(sql_result))!= NULL)	// Result Set���� 1���� �迭�� ������
		{
			printf("+ %3s   | %10s | %7s | %13s  +\n",sql_row[0],sql_row[1],sql_row[2],sql_row[3]);	// ����� �迭�� ���
		}
		printf("+-----------------------------------------------+\n");
		mysql_free_result(sql_result);		// Result Set �����Ѵ�
	}
}

void showWork (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row) {
	char *query = "select *from work order by wno";
	int state = 0;

	state = mysql_query(connection, query);
	if(state == 0)
	{
		printf("+----------------------------------------------------------------------+\n");
		printf("+  Wno |         Content         |  Time  |    Date    |   Cycle   +\n");
		printf("+----------------------------------------------------------------------+\n");
		sql_result = mysql_store_result(connection);			// Result Set�� ����
		while((sql_row = mysql_fetch_row(sql_result))!= NULL)	// Result Set���� 1���� �迭�� ������
		{
			printf("+ %3s  |  %s  |  %3s  |  %10s  |  %7s   +\n",sql_row[0],sql_row[1],sql_row[2],sql_row[3],sql_row[4]);	// ����� �迭�� ���
		}
		printf("+----------------------------------------------------------------------+\n");
		mysql_free_result(sql_result);		// Result Set �����Ѵ�
	}
}

void showRecruiting (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row) {
	char *query = "select *from recruiting order by vno, wno";
	int state = 0;

	state = mysql_query(connection, query);
	if(state == 0)
	{
		printf("+-----------------------------+\n");
		printf("+  Vno  |  Wno  |   state     +\n");
		printf("+-----------------------------+\n");
		sql_result = mysql_store_result(connection);			// Result Set�� ����
		while((sql_row = mysql_fetch_row(sql_result))!= NULL)	// Result Set���� 1���� �迭�� ������
		{
			printf("+ %3s   | %3s   |   %7s   +\n",sql_row[0],sql_row[1],sql_row[2]);	// ����� �迭�� ���
		}
		printf("+------------------------------+\n");
		mysql_free_result(sql_result);		// Result Set �����Ѵ�
	}
}

void searchResult (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, int n) {
	char query[200];
	int state = 0;
	
	/*
	  �����ں� �Ѻ���ð��� ����Ƚ���� ��ȸ�ϱ� ���� ���ǹ�
	  SELECT Recruiting.VNO, VName, sum(DoTime), count(Recruiting.WNO)
	  FROM Recruiting, Work, Volunteer
	  WHERE Recruiting.VNO=vno
	  AND Recruiting.WNO=Work.WNO
	  AND Recruiting.VNO=Volunteer.VNO
	  group by VNO;
	*/
	sprintf(query,"SELECT Recruiting.VNO, VName, sum(DoTime), count(Recruiting.WNO) FROM Recruiting, Work, Volunteer WHERE Recruiting.VNO='%d' AND Recruiting.WNO=Work.WNO AND Recruiting.VNO=Volunteer.VNO group by VNO", n);
	state = mysql_query(connection, query);
	if(state == 0)
	{
		printf("+--------------------------------------------------------+\n");
		printf("+  Vno  |    name    |  total hour | num of volunteering +\n");
		printf("+--------------------------------------------------------+\n");
		sql_result = mysql_store_result(connection);			// Result Set�� ����
		while((sql_row = mysql_fetch_row(sql_result))!= NULL)	// Result Set���� 1���� �迭�� ������
		{
			printf("+ %3s   | %10s |  %5s      |  %10s times   +\n",sql_row[0],sql_row[1],sql_row[2],sql_row[3]);	// ����� �迭�� ���
		}
		printf("+--------------------------------------------------------+\n");
		mysql_free_result(sql_result);		// Result Set �����Ѵ�
	}
}

void searchResult_work (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, int n) {
	char query[200];
	int state2 = 0;	
	/*�� �������� ����Ȱ�� ������ ��ȸ�ϱ� ���� ���ǹ�
	  SELECT Recruiting.wno, wcontent, dotime, date
	  FROM Recruiting, Work
	  WHERE Recruiting.vno=$vno
	  AND Recruiting.wno=Work.wno;
	*/
	sprintf(query,"SELECT Recruiting.wno, wcontent, dotime, date FROM Recruiting, Work WHERE Recruiting.vno='%d' AND Recruiting.wno=Work.wno", n);
	state2 = mysql_query(connection, query);

	if(state2 == 0)
	{
		printf("+------------------------------------------------------+\n");
		printf("+  Wno |         Content         |  Time  |    Date    +\n");
		printf("+------------------------------------------------------+\n");
		sql_result = mysql_store_result(connection);			// Result Set�� ����
		while((sql_row = mysql_fetch_row(sql_result))!= NULL)	// Result Set���� 1���� �迭�� ������
		{
			printf("+ %3s  |  %s  |  %3s  |  %10s  +\n",sql_row[0],sql_row[1],sql_row[2],sql_row[3]);	// ����� �迭�� ���
		}
		printf("+-------------------------------------------------------+\n");
		mysql_free_result(sql_result);		// Result Set �����Ѵ�
	}
}

int checkUser (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char* id, char* pw) {
	char query[200];
	int state;
	sprintf(query,"SELECT * FROM user_tb WHERE ID='%s' AND password='%s'", id, pw);
	state = mysql_query(connection, query);
	if(state == 0) {
		sql_result = mysql_store_result(connection);
		if((sql_row = mysql_fetch_row(sql_result))!= NULL) {
			printf("�α��� ���� \n");
			loginstate = 1;
		} else {
			printf("�α��� ���� \n");
			loginstate = 0;
		}
		mysql_free_result(sql_result);
	} else {
		printf("No Connection \n");
	}
	return state;
}

char* searchTitlekByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]) {
	char query[200];
	int state = 0;
	char* return_p;

	printf(query,"SELECT title FROM book_tb WHERE ISBN = '%s'", isbn);
	state = mysql_query(connection, query);
	if(state == 0){
		if((sql_row = mysql_fetch_row(sql_result))== NULL){
			return_p = " ";
			mysql_free_result(sql_result);		// Result Set �����Ѵ�
			return return_p;
		} else {
			return_p = sql_row[0];
			mysql_free_result(sql_result);		// Result Set �����Ѵ�
			return return_p;
		}
	} else
		printf("error!");
}

char* searchAuthorsByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]) {
	char query[200];
	int state = 0;
	char* return_p;

	printf(query,"SELECT authors FROM book_tb WHERE ISBN = '%s'", isbn);
	state = mysql_query(connection, query);
	if(state == 0){
		return_p = sql_row[0];
		mysql_free_result(sql_result);		// Result Set �����Ѵ�
		return return_p;
	} else
		printf("error!");
}
	
char* searchPublisherByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]) {
	char query[200];
	int state = 0;
	char* return_p;

	printf(query,"SELECT publisher FROM book_tb WHERE ISBN = '%s'", isbn);
	state = mysql_query(connection, query);
	if(state == 0){
		return_p = sql_row[0];
		mysql_free_result(sql_result);		// Result Set �����Ѵ�
		return return_p;
	} else
		printf("error!");
}
	
bool searchAvailabilityByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]) {
	char query[200];
	int state = 0;
	bool availability;

	printf(query,"SELECT availability FROM book_tb WHERE ISBN = '%s'", isbn);
	state = mysql_query(connection, query);
	if(state == 0){
		availability = sql_row[0];
		mysql_free_result(sql_result);		// Result Set �����Ѵ�
		return availability;
	} else
		printf("error!");
}