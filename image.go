// 10 june 2014
package main

import (
	"image"
	"sync"
)

// Image represents an in-memory image.
// It satisfies Go's draw.Image.
type Image struct {
	lock			sync.Mutex
	sysImage		*sysImage
}

// NewImage creates a new Image.
// It will initially be fully transparent.
func NewImage(width int, height int) *Image {
	return &Image{
		sysImage:		mkSysImage(width, height),
	}
}

// Close cleans up all resources and renders the image invalid.
func (i *Image) Close() {
	i.lock.Lock()
	defer i.lock.Unlock()

	i.sysImage.close()
	i.sysImage = nil
}

// Pen selects a pen for drawing lines and frames.
func (i *Image) Pen(p *Pen) {
	i.lock.Lock()
	defer i.lock.Unlock()
	p.lock.Lock()
	defer p.lock.Unlock()

	i.sysImage.selectPen(p)
}

// Line draws a line from (x0,y0) to (x1,y1) with the current Pen.
func (i *Image) Line(x0 int, y0 int, x1 int, y1 int) {
	i.lock.Lock()
	defer i.lock.Unlock()

	i.sysImage.line(x0, y0, x1, y1)
}

// Image produces a copy of i as a Go image.RGBA.
func (i *Image) Image() *image.RGBA {
	i.lock.Lock()
	defer i.lock.Unlock()

	return i.sysImage.toImage()
}