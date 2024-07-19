# Wombat SQLite
SQLite Forensic Tool

SQLite forensic tool which will include tagging and html reporting for specific columns from rows from tables in a db. The tool will parse the db separate from the wal to distinguish between the different states/versions of the database and hopefully provide it in a meaningful output which will provide an indication of what changes occurred, i.e. update, insert, etc.

The goal is to provide a free gui tool which runs on linux and provides reporting features and some of the features similar to those from Sanderson forensics. I will also eventually incorporate this into a viewer for [wombat forensics](https://github.com/pjrinaldi/wombatforensics).

## Repository Change
* I am no longer building code on github. I have moved my code to the website www.wombatforensics.com and am hosting my repositories on a vps using fossil for the repositories rather than git. I decided to stop using github due to all the AI crap and scraping code. My code isn't fancy or great, and it is free, but I just don't like the idea of scraping without my ok and since github is free, that is part of the price for free access. So I am leaving the historical bits of my repositories, but moving them all to fossil repositories. Feel free to check them out, they aren't as fancy or featureful as github, but it fits my needs.
