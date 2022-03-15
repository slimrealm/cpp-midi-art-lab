# MIDI Art Lab

MIDI Art Lab generates digital art based on notes played on a connected MIDI keyboard.

Pitch, velocity, and duration are captured, and those numeric values are used as parameters of a function for a particular drawing algorithm.  Currently, a single image is used and repeated throughout the drawing -- and the appearance of each new instance of that image changes based on the received parameters.  These drawing parameters include color, thickness, scale, angle, spacing between images, and number of images spawned over a given area.

The user can choose between three drawing algorithms (example image output for each shown below):

**1. Shifting lines of images**  
**2. Spiral**  
**3. Custom path which changes direction depending which notes are played**


<br>
<br>

## 1. Shifting line segments algorithm

![savedLeaf](https://user-images.githubusercontent.com/21164488/158470676-e7656b5a-23a3-4fd9-a53b-937ac6638fa7.png)

<br>

## 2. Spiral algorithm

![savedSpiral](https://user-images.githubusercontent.com/21164488/158470652-236f165b-4780-4b85-8750-11c1b5a39316.png)

<br>

## 3. Custom path algorithm

![savedForest](https://user-images.githubusercontent.com/21164488/158470692-dbd2fb12-0fde-47c8-9499-5f3c334c9055.png)

<br>
When algorithm finishes, a screenshot is captured and saved to an image file for user to do with as they please.
