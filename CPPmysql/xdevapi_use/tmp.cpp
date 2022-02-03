#include <iostream>
#include <iomanip>
#include <list>
#include <mysql-cppconn-8/mysqlx/xdevapi.h>

using namespace std;

// Scope controls life-time of objects such as session or schema

int main(void) {
    mysqlx::Session sess("192.168.122.34",33060,"dacao","111111");
    mysqlx::Schema db = sess.getSchema("library");
    mysqlx::Table tb = db.getTable("books");
    mysqlx::RowResult result = tb.select("*").execute();
    list<mysqlx::Row> rows = result.fetchAll();

    cout<<"id "<<"title "<<"author "<<"price "<<"quantity "<<endl;
    for(auto row : rows)
        cout<<row[0]<<" "<<row[1]<<" "<<row[2]<<" "<<row[3]<<" "<<row[4]<<endl;
}
