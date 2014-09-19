#!/bin/env python2

import bz2
import numpy as np
import struct
import sys

from numpy import random as rand
from scipy.fftpack import fft, fftfreq, fftshift
from scipy import signal as sig
from matplotlib import pyplot, animation

chunksize  = 131072           # read buffer size
targetfreq = 468000000        # target frequency
samplerate = 8000000          # samples per second
sampletime = 1.0 / samplerate # elapsed time between samples
fftsize    = 2048             # number of samples per transform

samplefreq = fftfreq(fftsize, sampletime)                  # get DFT sample frequencies
samplefreq = np.array((samplefreq + targetfreq) / 1000000) # offset by target frequency
samplefreq = fftshift(samplefreq)                          # shift zero-frequency component to center

with bz2.BZ2File('pager.iq.bz2', 'rb') as f:

	# get the filesize and reset position marker to beginning
	f.seek(0, 2)
	filesize = f.tell()
	f.seek(0, 0)

	def transform_chunk():
		# loop back if we ran out of data
		if filesize - f.tell() < chunksize * 2:
			f.seek(0, 0)

		# read a chunk of data and deinterleave I/Q data
		pair = struct.unpack('bb' * chunksize, f.read(chunksize * 2))
		real = np.array(pair[0::2])
		imag = np.array(pair[1::2])
		iq = np.array(real + 1j * imag, dtype=np.complex64)

		# perform a bunch of transforms within the chunk
		results = np.zeros(fftsize, dtype=np.complex64)
		numfft = int(chunksize / fftsize)
		for i in xrange(0, numfft):
			start = i * fftsize
			end = (i + 1) * fftsize 
			iqwindowed = iq[start:end] * sig.kaiser(fftsize, 14)
			results += fft(iqwindowed)
		results /= numfft
		return results


	# periodogram and waterfall buffer
	buff = np.zeros((100, fftsize))

	# create the figure
	fig, ax = pyplot.subplots(1, 1)
	ax.grid(True)
	ax.set_ylim(-4, 8)
	plot, = ax.plot(samplefreq, buff[0])

	# plotula rasa
	def init_fig():
		plot.set_ydata(np.zeros((fftsize,)))
		return plot,

	# get new values to plot every iteration
	def update_fig(*args):
		global buff
		buff = np.roll(buff, 1, 0)
		fft = fftshift(transform_chunk())
		buff[0] = np.log10(np.power(fft.real, 2) + np.power(fft.imag, 2))
		plot.set_ydata(buff[0])
		return plot,

	# animate this business
	anim = animation.FuncAnimation(fig, update_fig, blit=True, init_func=init_fig)
	pyplot.show()

