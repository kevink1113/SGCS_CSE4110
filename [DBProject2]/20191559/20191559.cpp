#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "mysql.h"
using namespace std;

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "mysql";	// TODO "mysql"
const char* db = "project";

MYSQL* connection = NULL;
MYSQL conn;
MYSQL_RES* sql_result;
MYSQL_ROW sql_row;


void query_print(const char* query) {
	using namespace std;
	int state = 0;

	state = mysql_query(connection, query);
	if (!state)
	{
		sql_result = mysql_store_result(connection);
		MYSQL_FIELD* field;
		int num_fields = mysql_num_fields(sql_result);

		// print line
		for (int i = 0; i < num_fields; i++) cout << "+-----------------";
		cout << "+\n";

		while ((field = mysql_fetch_field(sql_result))) {
			cout << "| " << left << setw(15) << field->name << " ";
		}
		cout << "|\n";

		// print line
		for (int i = 0; i < num_fields; i++) cout << "+-----------------";
		cout << "+\n";

		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		{
			for (int i = 0; i < num_fields; i++)
				cout << "| " << left << setw(14) << (sql_row[i] ? sql_row[i] : "NULL") << "  ";
			cout << "|\n";

			// print line
			for (int i = 0; i < num_fields; i++) cout << "+-----------------";
			cout << "+\n";
		}
		mysql_free_result(sql_result);
	}
}



int main(void) {

	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	else
	{
		printf("Connection Succeed\n");

		if (mysql_select_db(&conn, db))
		{
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}
		//////////// FILE INPUT /////////////
		FILE* fp = fopen("20191559.txt", "r");
		printf("\n====== File loading... ======\n\n");
		if (fp == NULL) {
			printf("File Open Error\n");
			return 1;
		}
		// input file with every line until EOF
		char line[300];
		while(fgets(line, 300, fp) != NULL) {
			//printf("%s", line);
			cout << "#";
			mysql_query(connection, line);
		}
		fclose(fp);

		printf("\n\n====== File load finished ======\n\n");

	}
	////////////// Prompt Start //////////////
	while(1) {
		printf("\n------- SELECT QUERY TYPES -------\n\n");
		printf("\t1. TYPE I\n");
		printf("\t2. TYPE II\n");
		printf("\t3. TYPE III\n");
		printf("\t4. TYPE IV\n");
		printf("\t5. TYPE V\n");
		printf("\t0. QUIT\n");
		printf("\n\n");
		printf("Query Type : ");
		int type;
		cin >> type;

		if(!type) break;
		else if (type ==1) {
			int truck;
			cout << "\nTruck number: ";
			cin >> truck;

			if (truck != 1) {
				cout << "Truck LT";
				cout.width(4); cout.fill('0');
				cout << truck << " is not destroyed. (hint: 1)" << endl;
				cout.fill(' ');
				continue;
			}
			while(1) {
				

				printf("\n\n------- Subtypes in TYPE I -------\n\n");
				printf("\t1. TYPE I-1.\n");
				printf("\t2. TYPE I-2.\n");
				printf("\t3. TYPE I-3.\n");
				printf("\nSubtype : ");
				cin >> type;

				if (!type) break;
				else if (type==1) {
					printf("\n---- TYPE I-1 ----\n");
					printf("** Find all customers who had a package on the truck at the time of the crash. **\n");

					const char* query =
						"SELECT DISTINCT c.* \
						FROM Customer c \
						JOIN Package p ON c.customer_id = p.customer_id \
						JOIN Contain cn ON p.package_id = cn.package_id \
						JOIN Shipment s ON cn.shipment_id = s.shipment_id AND cn.shipment_enum = 6 \
						JOIN Location l ON s.location_id = l.location_id \
						WHERE l.location_type = 'truck' AND l.location_id = 'LT0001';";

					query_print(query);


				}
				else if (type==2) {
					printf("\n---- TYPE I-2 ----\n");
					printf("** Find all recipients who had a package on that truck at the time of the crash. **\n");
				
					const char* query = 
						"SELECT DISTINCT r.* \
						FROM Recipient r \
						JOIN Package p ON r.recipient_id = p.recipient_id \
						JOIN Contain cn ON p.package_id = cn.package_id \
						JOIN Shipment s ON cn.shipment_id = s.shipment_id AND cn.shipment_enum = 6 \
						JOIN Location l ON s.location_id = l.location_id \
						WHERE l.location_type = 'truck' AND l.location_id = 'LT0001';";
					query_print(query);
				}
				else if (type == 3) {
					printf("\n---- TYPE I-3 ----\n");
					printf("** Find the last successful delivery by that truck prior to the crash. **\n");

					const char* query =
						"SELECT s1.* \
						FROM Shipment s1 \
						JOIN Location l1 ON s1.location_id = l1.location_id \
						WHERE l1.location_type = 'truck' AND l1.location_id = 'LT0001' \
						AND s1.timestamp < (SELECT s2.timestamp FROM Shipment s2 WHERE s2.status = 'Crash') \
						ORDER BY s1.timestamp desc \
						LIMIT 1;";
					query_print(query);
				}
				else {
					printf("Wrong Type Input\n");
					continue;
				}
			}
		}
		else if (type==2) {
			printf("\n\n---- TYPE II ----\n");
			printf("** Find the customer who has shipped the most packages in the past year. ** (Hint: 2023)\n");
			int year;
			cout << "\nYear: ";
			cin >> year;
			char query[500];
			sprintf(query, 
				"SELECT c.*, COUNT(p.package_id) AS package_count \
				FROM Customer c \
				JOIN Package p ON c.customer_id = p.customer_id \
				WHERE YEAR(p.ship_date) = %d \
				GROUP BY c.customer_id \
				ORDER BY package_count DESC \
				LIMIT 1;", year);
			query_print(query);

		}
		else if (type==3) {
			printf("\n---- TYPE III ----\n");
			printf("** Find the customer who has spent the most money on shipping in the past year. ** (Hint: 2023)\n");
			int year;
			cout << "\nYear: ";
			cin >> year;
			char query[500];
			sprintf(query,
				"SELECT b.customer_id, c.name, SUM(b.amount) as total_amount \
				FROM Billing b \
				JOIN Customer c ON b.customer_id = c.customer_id \
				WHERE YEAR(b.billing_date) = %d \
				GROUP BY b.customer_id, c.name \
				ORDER BY total_amount DESC \
				LIMIT 1;", year);
			query_print(query);
		}
		else if(type==4) {
			printf("\n---- TYPE IV ----\n");
			printf("** Find the packages that were not delivered within the promised time. **\n");
			const char* query =
				"SELECT P.package_id, P.delivery_date as promised_time, S.shipment_id, S.timestamp as actual_delivery \
				FROM Package P \
				JOIN Contain C ON P.package_id = C.package_id \
				JOIN Shipment S ON C.shipment_id = S.shipment_id AND C.shipment_enum = S.shipment_enum \
				WHERE S.status = 'delivered' AND S.timestamp > P.delivery_date;";

			query_print(query);
		}
		else if(type==5) {
			printf("\n---- TYPE V ----\n");
			printf("** Generate the bill for each customer for the past month. **\n");
			int year, month;
			cout << "\nYear Month (hint: 2019 1): ";
			cin >> year >> month;
			char query[500];
			int bill;
			cout << "\nWhich type of bill do you want?" << endl;
			cout << "\t1. A simple bill: customer, address, and amount owed." << endl;
			cout << "\t2. A bill listing charges by type of service." << endl;
			cout << "\t3. An itemize billing listing each individual shipment and the charges for it." << endl;
			cout << "\nBill type: ";
			cin >> bill;
			switch (bill) {
			case 1:
				sprintf(query,
					"SELECT b.customer_id, c.name, c.address, SUM(b.amount) as total_amount \
					FROM Billing b \
					JOIN Customer c ON b.customer_id = c.customer_id \
					WHERE YEAR(b.billing_date) = %d AND MONTH(b.billing_date) = %d \
					GROUP BY b.customer_id, c.name, c.address;", year, month);
				query_print(query);
				break;
			case 2:
				sprintf(query,
					"SELECT b.customer_id, c.name, p.service, SUM(b.amount) as total_amount \
					FROM Billing b \
					JOIN Customer c ON b.customer_id = c.customer_id \
					JOIN Package p ON b.package_id = p.package_id \
					WHERE YEAR(b.billing_date) = %d AND MONTH(b.billing_date) = %d \
					GROUP BY b.customer_id, c.name, p.service;", year, month);
				query_print(query);
				break;
			case 3:
				sprintf(query,
					"SELECT b.billing_id, c.name, b.package_id, p.service, b.amount \
					FROM Billing b \
					JOIN Customer c ON b.customer_id = c.customer_id \
					JOIN Package p ON b.package_id = p.package_id \
					WHERE YEAR(b.billing_date) = %d AND MONTH(b.billing_date) = %d;", year, month);
				query_print(query);
				break;
			}


		}
		else {
			printf("Wrong Type Input\n");
			continue;
		}
		//printf("---- TYPE V ----\n\n");
		//printf("** Find the top k brands by unit sales by the year**\n");
		//printf(" Which K? : 3\n");
	}

	//////////// FILE INPUT /////////////
	FILE* fp2 = fopen("20191559_drop.txt", "r");
	printf("\n====== File loading...(drop) ======\n\n");
	if (fp2 == NULL) {
		printf("File Open Error\n");
		return 1;
	}
	// input file with every line until EOF
	char line[300];
	while (fgets(line, 300, fp2) != NULL) {
		cout << "#";
		mysql_query(connection, line);
	}
	printf("\n\n===== Table drop finished =====\n");
	fclose(fp2);

	mysql_close(connection);

	return 0;
}