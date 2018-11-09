FROM plusminus/qt:latest

WORKDIR /tmp
COPY CMakeLists.txt ./

RUN apt-get update -yqq && apt-get dist-upgrade -yqq && apt-get install -yqq \
    	    libxt-dev \
	    libtiff5-dev \
	    git

RUN cmake . && make