/********************************************************************
 *																	*
 *					2014 Software Engineering HW#2  				*
 *																	*
 *	1114606 유수정, 1110828 박지수, 1113298 남지수, 1111176 황수진	*
 *		  			<<Library Information System>>					*
 *																	*
 ********************************************************************/

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

//int loginstate = 0; //0이면 로그인 안 된 상태, 1이면 로그인 된 상태

int checkUser (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char* id, char* pw);
void insertBookInfo (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row);
int deleteBookInfo (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row);
void editBookInfo (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row);
MYSQL_ROW searchTitleByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]);
int updateTitleByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]);
int updateAuthorsByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]);
int updatePublisherByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]);
MYSQL_ROW searchAvailabilityByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]);
int updateAvailability(MYSQL*connection, bool availability, char isbn[]);

int searchBookInfoByTitle (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char title[]);
int showBookInfoById (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char id[]);

int main(int argc, CHAR* argv[])
{
	MYSQL *connection = NULL;
	MYSQL conn;
	MYSQL_RES *sql_result = NULL;
    MYSQL_ROW sql_row = NULL;

	int signin_state;

	char id[10];
	char password[10];
	char sname[30];
	char department[30];

	char title[30];
	char query[200];
	int state;
	int i = 0;
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
				//사용자 데이터가 user_tb에 있는지 확인
				printf("<< Sign In >>\n");
				printf("your ID : ");
				scanf("%s", id);
				printf("your password : ");
				scanf("%s", password);

				signin_state = checkUser(&conn, sql_result, sql_row, id, password);

				//signin_state가 1이면 로그인이 된 상태!
				if(signin_state == 1) {
					printf("You Signed in! Welcome to LIS!\n\n");
					
					/* 유저타입이 사서인지 학생인지 판별 */
					//id가 librarian이면 사서
					if (strcmp(id , "librarian") ==0){
			
						while (true){
							printf("<< Library Information System >>\n\n");
							printf("  1. Register New Book\n");
							printf("  2. Edit Book Information\n");
							printf("  3. Delete Book Information\n  >>");
							printf("  0. Back\n\n  >>");
							scanf("%d",&select_num);
							if (select_num == 1){ //책추가
								insertBookInfo(&conn, sql_result, sql_row);
			
							} else if (select_num == 2){  //책 정보 수정
								editBookInfo(&conn, sql_result, sql_row);
					
							} else if (select_num == 3){ //책 삭제
								state = deleteBookInfo(&conn, sql_result, sql_row);
								if(state == 0)
									printf("Successfully Deleted!\n");
								else
									printf("Fail to Delete. Try Agian.\n");
							} else if (select_num == 0){
								signin_state = 0;
								break;
							}else 
								printf("Please reenter number which is correct.");
						}
					}
					//유저가 입력한 id가 "librarian"이 아니면 무조건 학생임
					else {
						while (true){
							printf("<< Library Information System >>\n\n");
							printf("  1. Search a Book\n");
							printf("  2. Show Rented Books\n");
							printf("  0. Back\n\n  >>");
							scanf("%d",&select_num);
							if (select_num == 1){ //책 검색
								printf("검색할 책 제목을 입력하세요.\n");
								scanf("%s",&title);
								searchBookInfoByTitle(&conn, sql_result, sql_row, title);
							} else if (select_num == 2){  //빌린 책 보여주기
								showBookInfoById(&conn, sql_result, sql_row, id);	
							} else if (select_num == 0){
								signin_state = 0;
								break;
							}else 
								printf("Please reenter number which is correct.");
						}
					}
				} else if (signin_state == 0) {
					printf("Sign in failed! Try again.\n\n");
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
			
		}	
	}
	mysql_free_result(sql_result);
	mysql_close(connection);
	return 0;
}
int checkUser (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char* id, char* pw) {
	char query[200];
	int loginstate;
	int state; //단순히 쿼리가 실행되었는지 아닌지 알려줌, 실행되면 0을 반환
	sprintf(query,"SELECT * FROM user_tb WHERE ID='%s' AND password='%s'", id, pw);
	state = mysql_query(connection, query);
	if(state == 0) {
		sql_result = mysql_store_result(connection);
		if((sql_row = mysql_fetch_row(sql_result))!= NULL) {
			printf("checkUser : 로그인 성공 \n\n");
			loginstate = 1;
		} else {
			printf("checkUser : 로그인 실패 \n\n");
			loginstate = 0;
		}
		mysql_free_result(sql_result);
	}
	else {
		printf("checkUser 쿼리수행안됨 \n");
	}
	return loginstate;
}

void insertBookInfo (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row) {
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
	//fflush(stdin);
	sprintf(query,"insert into book_tb(title, authors, publisher, ISBN) values('%s', '%s', '%s', '%s')", title, authors, publisher, isbn);
	mysql_query (connection, query);
	printf("\nYou successfully completed registration of the new book!\n\n");

}

void editBookInfo (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row){
	int select_edit;
	int select_avail;
	int state;

	MYSQL_ROW result_row;

	char id[20];
	char title[30]; 
	char authors[10];
	char publisher[20];
	char isbn[18];
	char newisbn[18];
	char query[200];
	bool availability;	

	printf("<< Edit Book Information >>\n\n");
	printf("Please enter ISBN of the book which you want to edit information.\n");
	printf("ISBN : ");
	scanf("%s", isbn);
	fflush(stdin);
	result_row = searchTitleByISBN(connection, sql_result, sql_row, isbn);
	if(result_row == NULL) {
		printf("THE ISBN IS NOT EXIST. TRY AGAIN.\n");
		return;
	}
	printf("searchTitleByISBN result : '%s'\n",result_row[0]);
	if(result_row[0] != NULL) {
		while(1){
			printf("What do you want to edit the book's information?\n");
			printf("1. Title\n");
			printf("2. Authors\n");
			printf("3. Publisher\n");
			printf("4. ISBN\n");
			printf("5. Availability\n");
			printf("0. Back\n");
			fflush(stdin);
			scanf("%d", &select_edit);
			if(select_edit == 1){ //책 title 편집
				state = updateTitleByISBN(connection, sql_result, sql_row, isbn);
				if(state = 0)
					printf("You successfully changed information of the book!\n\n");
				else
					printf("You fail to update. Try again.\n\n");
			} else if (select_edit == 2){  //책 저자 편집
				state = updateAuthorsByISBN (connection, sql_result, sql_row, isbn);
				if(state == 0)
					printf("You successfully changed information of the book!\n\n");
				else
					printf("You fail to update. Try again.");
			} else if (select_edit == 3){  //책 출판사 편집
				state = updatePublisherByISBN (connection, sql_result, sql_row, isbn);
				if(state == 0)
					printf("You successfully changed information of the book!\n\n");
				else
					printf("Fail to update. Try again.");
			} else if (select_edit == 4){  //책 ISBN 편집
				printf("This book's ISBN is %s. Enter new ISBN of the book.\n>> ", isbn);
				scanf("%s", newisbn);
				sprintf(query,"update book_tb set ISBN = '%s' where ISBN = '%s'", newisbn, isbn);
				state = mysql_query(connection, query);
				if(state == 0) {
					printf("You successfully changed information of the book!\n\n");
					return;
				}
				else
					printf("Fail to update. Try again.");
			} else if (select_edit == 5){  //책 대여여부 편집
				result_row = searchAvailabilityByISBN (connection, sql_result, sql_row, isbn);
				if(*result_row[0]=='1')
					availability = true;
				else
					availability = false;
					//printf("searchAvailabilityByISBN result : '%d'\n",availability);
					updateAvailability(connection, availability, isbn);
				} else if (select_edit == 0){//돌아가기
					mysql_free_result(sql_result);
					return;
			} else
				printf("Please reenter number which is correct.");
		}
	}
}

int deleteBookInfo (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row){
	
	char isbn[18];
	MYSQL_ROW result;
	char* bookname;
	char query[200];
	int select_delete;
	int state;

	while(1){
		printf("<< Delete Book Information >>\n\n");
		printf("Please enter ISBN of the book which you want to delete.\n");
		printf("ISBN : ");
		fflush(stdin);
		scanf("%s", isbn);
		result = searchTitleByISBN (connection, sql_result, sql_row, isbn);

		if(result[0] == NULL) {
			printf("The book which you search by ISBN doesn't exist in this system.\n");
			return 1;
		} else {
			printf("The book name which you selected is %s.\n",result[0]);
			printf("Do you really want to delete?\n");
			printf("1. Yes\n");
			printf("2. No\n");
			fflush(stdin);
			scanf("%d", &select_delete);

			if (select_delete == 1){
				sprintf(query,"DELETE FROM book_tb WHERE ISBN = '%s'", isbn);
				state = mysql_query(connection, query);
				if(state == 0)
					return 0;
				else
					return 1;
			} else if (select_delete == 2)
				break;
			else
				printf("Please reenter number which is correct.\n");
		}
	}
}

MYSQL_ROW searchTitleByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]) {
	char query[200];
	int state = 0;
	char title[50];
	MYSQL_ROW result;

	sprintf(query,"SELECT title FROM book_tb WHERE ISBN = '%s'", isbn);
	state = mysql_query(connection, query);
	if(state == 0)
	{
		sql_result = mysql_store_result(connection);			// Result Set에 저장
		result = mysql_fetch_row(sql_result);
		mysql_free_result(sql_result);// Result Set 해제한다
	}

	return result;
}

int updateTitleByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]) {
	char query[200];
	int state = 0;
	char title[50];
	MYSQL_ROW result;

	result = searchTitleByISBN (connection, sql_result, sql_row, isbn);
	if(result[0] == NULL) {
		printf("The book which you search by ISBN doesn't exist in this system.\n");
		return 1;
	} else {
		printf("This book's title is %s. Enter new title of the book : \n", result[0]);
		scanf("%s", title);
		sprintf(query,"update book_tb set title = '%s' where ISBN = '%s'", title, isbn);
		state = mysql_query(connection, query);
		if(state == 0)
			return 0;
		else
			return 1;
	}
}


int updateAuthorsByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]) {
	char query[200];
	int state = 0;
	char authors[30];
	MYSQL_ROW result;

	sprintf(query,"SELECT authors FROM book_tb WHERE ISBN = '%s'", isbn);
	state = mysql_query(connection, query);
	if(state == 0)
	{
		sql_result = mysql_store_result(connection);			// Result Set에 저장
		result = mysql_fetch_row(sql_result);
		mysql_free_result(sql_result);// Result Set 해제한다
	}
	if(result[0] == NULL) {
		printf("The book which you search by ISBN doesn't exist in this system.\n");
		return 1;
	} else {
		printf("This book's authors are %s. Enter new authors of the book : \n", result[0]);
		scanf("%s", authors);
		sprintf(query,"update book_tb set authors = '%s' where ISBN = '%s'", authors, isbn);
		state = mysql_query(connection, query);
		if(state == 0)
			return 0;
		else
			return 1;
	}
}
	
int updatePublisherByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]) {
	char query[200];
	int state = 0;
	char publisher[30];
	MYSQL_ROW result;

	sprintf(query,"SELECT publisher FROM book_tb WHERE ISBN = '%s'", isbn);
	state = mysql_query(connection, query);
	if(state == 0)
	{
		sql_result = mysql_store_result(connection);			// Result Set에 저장
		result = mysql_fetch_row(sql_result);
		mysql_free_result(sql_result);// Result Set 해제한다
	}
	if(result[0] == NULL) {
		printf("The book which you search by ISBN doesn't exist in this system.\n");
		return 1;
	} else {
		printf("This book's publisher is %s. Enter new publisher of the book : \n", result[0]);
		scanf("%s", publisher);
		sprintf(query,"update book_tb set publisher = '%s' where ISBN = '%s'", publisher, isbn);
		state = mysql_query(connection, query);
		if(state == 0)
			return 0;
		else
			return 1;
	}
}
	
MYSQL_ROW searchAvailabilityByISBN (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char isbn[]) {
	char query[200];
	int state = 0;
	MYSQL_ROW result;

	sprintf(query,"SELECT availability FROM book_tb WHERE ISBN = '%s'", isbn);
	state = mysql_query(connection, query);
	if(state == 0)
	{
		sql_result = mysql_store_result(connection);			// Result Set에 저장
		if( mysql_num_fields(sql_result) == 0) {
			printf("THE ISBN IS NOT EXIST.\n");
		}
		result = mysql_fetch_row(sql_result);
		mysql_free_result(sql_result);// Result Set 해제한다
	}
	return result;
}

int searchBookInfoByTitle (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char title[]) {
	char query[200];
	int state = 0;
	sprintf(query,"SELECT title, authors, publisher, ISBN, availability FROM book_tb WHERE title like \'%%%s%%\'", title); 
	state = mysql_query(connection, query);
	if(state == 0) {
		printf("+---------------------------------------------------------------------------+\n");
		printf("+    title    |    authors   |   publisher  |      ISBN      | availability +\n");
		printf("+---------------------------------------------------------------------------+\n");
		sql_result = mysql_store_result(connection);			// Result Set에 저장
		while((sql_row = mysql_fetch_row(sql_result))!= NULL)	// Result Set에서 1개씩 배열에 가져옴
		{
			printf("+ %10s  |  %10s  |  %10s  |  %10s  |  %10s  +\n",sql_row[0],sql_row[1],sql_row[2],sql_row[3],sql_row[4]);	// 저장된 배열을 출력
		}
		printf("+---------------------------------------------------------------------------+\n");
		mysql_free_result(sql_result);		// Result Set 해제한다
		return 0;
	} else {
		return 1;
	}
}

int showBookInfoById (MYSQL*connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row, char id[]) {
	char query[200];
	int state = 0;
	sprintf(query,"SELECT title, authors, publisher, ISBN FROM book_tb WHERE renting_student='%s'", id);
	state = mysql_query(connection, query);
	if(state == 0) {
		printf("+------------------------------------------------------------+\n");
		printf("+    title    |    authors   |   publisher  |      ISBN      +\n");
		printf("+------------------------------------------------------------+\n");
		sql_result = mysql_store_result(connection);			// Result Set에 저장
		while((sql_row = mysql_fetch_row(sql_result))!= NULL)	// Result Set에서 1개씩 배열에 가져옴
		{
			printf("+ %10s  |  %10s  |  %10s  |  %10s  +\n",sql_row[0],sql_row[1],sql_row[2],sql_row[3]);	// 저장된 배열을 출력
		}
		printf("+------------------------------------------------------------+\n");
		mysql_free_result(sql_result);		// Result Set 해제한다
		return 0;
	} else
		return 1;
}

int updateAvailability(MYSQL*connection, bool availability, char isbn[]) {
	int select_avail;
	char query[200];
	char id[20];

	if(availability == true){
		while(1){
			printf("The book can be rented. Do you want to change it?\n");
			printf("1. Yes\n");
			printf("2. No\n>> ");
			scanf("%d",&select_avail);
			if(select_avail == 1){
				printf("Enter ID of the renting student.\n");
				scanf("%s", id);
				sprintf(query,"update book_tb set availability = '0', renting_student = '%s' where ISBN = '%s'", id, isbn);
				mysql_query (connection, query);
				printf("You successfully changed information of the book!\n\n");
				return 0;
			} else if (select_avail == 2){
				return 0;
			} else
				printf("Please reenter number which is correct.");
		}
	} else{
		while(1){
			printf("The book have been rented. Do you want to change it?\n");
			printf("1. Yes\n");
			printf("2. No\n>> ");
			scanf("%d",&select_avail);
			if(select_avail == 1){
				sprintf(query,"update book_tb set availability = '1', renting_student = NULL where ISBN = '%s'", isbn);
				mysql_query (connection, query);
				printf("You successfully changed information of the book!\n\n");
				return 0;
			} else if (select_avail == 2){
				return 0;
			} else
				printf("Please reenter number which is correct.");
		}
	}
}