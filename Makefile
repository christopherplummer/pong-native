.PHONY: linux run-linux detect clean

linux:
	./linux/build.sh

run-linux:
	./linux/run.sh

detect:
	./linux/detect_linux_ui.sh

clean:
	rm -rf ./linux/build ./linux/pong_native
