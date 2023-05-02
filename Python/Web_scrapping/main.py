import requests
from bs4 import BeautifulSoup

#Define parameters
url = "https://www.selvacorealty.com/our-team/"
headers = {'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:55.0) Gecko/20100101 Firefox/55.0',}

#Generate GET request
result = requests.get(url, headers=headers)
#Create soup with html data
soup = BeautifulSoup(result.content, "html.parser")
print(soup.title )
