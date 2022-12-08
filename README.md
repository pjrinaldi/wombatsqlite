# Wombat SQLite
SQLite Forensic Tool

SQLite forensic tool which will include tagging and html reporting for specific columns from rows from tables in a db. The tool will parse the db separate from the wal to distinguish between the different states/versions of the database and hopefully provide it in a meaningful output which will provide an indication of what changes occurred, i.e. update, insert, etc.

The goal is to provide a free gui tool which runs on linux and provides reporting features and some of the features similar to those from Sanderson forensics. I will also eventually incorporate this into a viewer for [wombat forensics](https://github.com/pjrinaldi/wombatforensics).
