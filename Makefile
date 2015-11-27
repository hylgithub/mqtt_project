#main Makefile

all:
	cd ./lib && make all
	cd ./service && make all
	cd ./client/pub_client && make all
	cd ./client/sub_client && make all
clean:
	cd ./lib && make clean
	cd ./service && make clean
	cd ./client/pub_client && make clean
	cd ./client/sub_client && make clean
