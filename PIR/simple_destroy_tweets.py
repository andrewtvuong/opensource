#!/usr/bin/env python3

import datetime
import tweepy
import re

timestamp = datetime.datetime.now()
print('time start {:%Y-%b-%d %H:%M:%S}'.format(timestamp))

app_key = 'appkey'
app_secret = 'appsecret'
oath_token = 'oathtoken'
oath_token_secret = 'oathsecret'

auth = tweepy.OAuthHandler(app_key, app_secret)
auth.set_access_token(oath_token, oath_token_secret)

api = tweepy.API(auth, wait_on_rate_limit=True, wait_on_rate_limit_notify=True, compression=True)

# number of latest self tweets to delete, so 50 would be your 50 latest tweets.
num_to_destroy = 50

for status in tweepy.Cursor(api.user_timeline).items(num_to_destroy):
    statusid = re.search('status\/([\d]*)', str(status._json)).group(1)
    print(statusid)
    api.destroy_status(statusid)
