const mysql = require("mysql");
const bbPromise = require('bluebird');

class KV_Store {
    constructor(h, u, pwd, tbl) {
        this.con = bbPromise.promisifyAll(mysql.createConnection({
            host: h,
            user: u,
            password: pwd,
            database: "unsecurekv"
        }));
        if (tbl) {
            this.table = tbl;
        } else {
            this.table = 'kvstore';
        }
    }



    init() {
        const showTablesSql = `
SHOW TABLES like ?;
        `;
        const createTableSql = `
CREATE TABLE ${this.table} (
    rowkey VARCHAR(256) NOT NULL,
    rowvalues LONGBLOB,
    PRIMARY KEY (rowkey)
);
            `;

        console.log("** DEBUG: Call to init.");
        return this.con.connectAsync()
            .then(() => console.log("** DEBUG: K-V store connected successfully."))
            .then(() => this.con.queryAsync(showTablesSql, [this.table]))
            .then(
                (result) => {
                    console.log("** DEBUG: Query successful - getting list of tables in database.");
                    if (result.length === 0) {
                        console.log("** DEBUG: No table in database. Calling query to create table.");
                        return this.con.queryAsync(createTableSql)
                            .then(() => console.log("** DEBUG: Query successful - creating table."));
                    } else {
                        console.log("** DEBUG: table already exists in the database. No need to create it.");
                    }
                })
            .catch(
                (err) => {
                    console.log("*** DEBUG: Failed in init");
                    return bbPromise.reject(err);
                }
            );
    }

    close() {
        console.log("** DEBUG: Call to close.");
        return this.con.endAsync()
            .then(() => console.log("** DEBUG: K-V store connection closed successfully."))
            .catch((err) => {
                console.log("** DEBUG: Failed closing the database connection.");
                return bbPromise.reject(err);
            });
    }

    put (k, v) {
        const putQuerySql = `
INSERT INTO ${this.table} (rowkey,rowvalues)
    VALUES (?, ?)
    ON DUPLICATE KEY UPDATE
        rowvalues = VALUES(rowvalues);
        `;
        console.log("** DEBUG: Call to put.");
        console.log("** DEBUG:   Key:   " + k + ".");
        // console.log("** DEBUG:   Value: " + v + ".");

        return this.con.queryAsync(putQuerySql, [k, v])
            .then((result) => {
                console.log("** DEBUG: Query successful - inserting values.");
                console.log("** DEBUG: Query result:");
                console.log(result);
                console.log("** DEBUG: Query result />");
            })
            .catch( (err) => {
                console.log("** DEBUG: Query failed - inserting values.");
                return bbPromise.reject(err);
            })
    }

    get (k) {
        const getQuerySql = `
SELECT rowvalues
FROM ${this.table}
WHERE rowkey = ?;
    `;
        console.log("** DEBUG: Call to get.");
        console.log("** DEBUG:   Key:   " + k + ".");

        return this.con.queryAsync(getQuerySql, [k])
            .then((result) => {
                console.log("** DEBUG: Query successful - getting values.");
                /* console.log("** DEBUG: Query result:");
                console.log(result);
                console.log("** DEBUG: Query result />"); */

                if (result.length === 0) return "";
                if (result.length === 1) return result[0]["rowvalues"];
                if (result.length > 1) return bbPromise.reject("Inconsistent KeyValueStore");
            })
            .catch((err) => {
                console.log("** DEBUG: Query failed - getting values.");
                return bbPromise.reject(err);
            });
    }

    keys() {
        const keysQuerySql = `
SELECT rowkey
FROM ${this.table};
    `;
        console.log("** DEBUG: Call to keys.");

        return this.con.queryAsync(keysQuerySql)
            .then((result) => {
                console.log("** DEBUG: Query successful - getting keys.");
                console.log("** DEBUG: Query result:");
                console.log(result);
                console.log("** DEBUG: Query result />");

                return result.map(x => x["rowkey"]);
            })
            .catch((err) => {
                console.log("** DEBUG: Query failed - getting keys.");
                return bbPromise.reject(err);
            });
    }

    del(k) {
        const deleteQuerySql = `
DELETE FROM ${this.table}
WHERE rowkey = ?;
    `;
        console.log("** DEBUG: Call to delete.");
        console.log("** DEBUG:   Key:   " + k + ".");

        return this.con.queryAsync(deleteQuerySql, [k])
            .then((result) => {
                console.log("** DEBUG: Query successful - deleting key.");
                console.log("** DEBUG: Query result:");
                console.log(result);
                console.log("** DEBUG: Query result />");
            })
            .catch((err) => {
                console.log("** DEBUG: Query failed - deleting key.");
                return bbPromise.reject(err);
            });
    }

    entries() {
        const entriesQuerySql = `
SELECT rowkey, rowvalues
FROM ${this.table};
    `;
        console.log("** DEBUG: Call to entries.");

        return this.con.queryAsync(entriesQuerySql)
            .then((result) => {
                console.log("** DEBUG: Query successful - getting entries.");
                console.log("** DEBUG: Query result:");
                console.log(result);
                console.log("** DEBUG: Query result />");

                return result.map( x => {
                    return {
                        'key': x["rowkey"],
                        'val': x["rowvalues"],
                    }})
            })
            .catch((err) => {
                console.log("** DEBUG: Query failed - getting entries.");
                return bbPromise.reject(err);
            });
    }
}

module.exports.KV_Store = KV_Store;

/* ************************
 *          Tests
 * ************************ */

if (process.argv[2] === "test") {

    const kv = new KV_Store(
        process.argv[3],
        process.argv[4],
        process.argv[5]
    );

    bbPromise.resolve()
        .then(() => console.log("************************"))
        .then(() => console.log("Store Init Test:"))
        .then(() => kv.init())
        .catch((err) => console.log("##### Init Test Error: " + err))
        .then(() => console.log("************************"))

        .then(() => console.log("**"))

        .then(() => console.log("************************"))
        .then(() => console.log("Put Test:"))
        .then(() => kv.entries()).then((result) => console.log(result))
        .then(() => kv.put('a', 'value for a'))
        .then(() => kv.entries()).then((result) => console.log(result))
        .then(() => kv.put('b', 'a value for b'))
        .then(() => kv.entries()).then((result) => console.log(result))
        .then(() => kv.put('a', 'another value for a'))
        .then(() => kv.entries()).then((result) => console.log(result))
        .catch((err) => console.log("##### Put Test Error: " + err))
        .then(() => console.log("************************"))

        .then(() => console.log("**"))

        .then(() => console.log("************************"))
        .then(() => console.log("Get Test:"))
        .then(() => kv.get('a'))
        .then((result) => console.log("Stored value for key a: " + result))
        .then(() => kv.get('b'))
        .then((result) => console.log("Stored value for key b: " + result))
        .catch((err) => console.log("##### Get Test Error: " + err))
        .then(() => console.log("************************"))

        .then(() => console.log("**"))

        .then(() => console.log("************************"))
        .then(() => console.log("Keys Test:"))
        .then(() => kv.keys())
        .then((result) => console.log("Stored keys: " + result))
        .catch((err) => console.log("##### Keys Test Error: " + err))
        .then(() => console.log("************************"))

        .then(() => console.log("**"))

        .then(() => console.log("************************"))
        .then(() => console.log("Entries Test:"))
        .then(() => kv.entries())
        .then((result) => console.log("Stored entries: " + result))
        .catch((err) => console.log("##### Entries Test Error: " + err))
        .then(() => console.log("************************"))

        .then(() => console.log("**"))

        .then(() => console.log("************************"))
        .then(() => console.log("Delete Test:"))
        .then(() => kv.entries()).then((result) => console.log(result))
        .then(() => kv.del('a'))
        .then(() => kv.entries()).then((result) => console.log(result))
        .catch((err) => console.log("##### Delete Test Error: " + err))
        .then(() => console.log("************************"))

        .then(() => console.log("**"))

        .then(() => console.log("************************"))
        .then(() => console.log("Close Test:"))
        .then(() => console.log("Connection state : " + kv.con.state))
        .then(() => kv.close())
        .then(() => console.log("Connection state : " + kv.con.state))
        .catch((err) => console.log("##### Close Test Error: " + err))
        .then(() => console.log("************************"));
}
