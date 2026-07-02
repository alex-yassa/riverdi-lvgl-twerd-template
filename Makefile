.PHONY: all clean cm4 cm7

all: cm4 cm7

cm4:
	$(MAKE) -C STM32CubeIDE/CM4/Release all

cm7:
	$(MAKE) -C STM32CubeIDE/CM7/Release all

clean:
	$(MAKE) -C STM32CubeIDE/CM4/Release clean
	$(MAKE) -C STM32CubeIDE/CM7/Release clean
