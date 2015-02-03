LIBS= `pkg-config --libs opencv`
CFLAGS= `pkg-config --cflags opencv`
objects= multi_color.o

MultiColor: $(objects) 
	gcc $(objects) -o MultiColor $(LIBS)$(CFLAGS)

.PHONY: clean
clean:
	rm MultiColor $(objects)
