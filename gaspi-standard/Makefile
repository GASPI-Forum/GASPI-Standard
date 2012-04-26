all:
	make ps docs

cleanall:
	make cleanps cleandocs

ps:
	make -C ./tex ps

cleanps:
	make -C ./tex clean

docs:
	make -C ./doc docs

cleandocs:
	make -C ./doc cleandocs

examples:
	all2all_write
	all2all_read
