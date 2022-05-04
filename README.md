# MIDI Art Lab

MIDI Art Lab generates digital art based on notes played on a connected MIDI keyboard.

Pitch, velocity, and duration are captured, and those numeric values are used as parameters of a function for a particular drawing algorithm.  Currently, a single image is used and repeated throughout the drawing -- and the appearance of each new instance of that image changes based on the received parameters.  These drawing parameters include color, transparency, thickness, scale, angle, spacing between images, and number of images spawned over a given area.

The user can choose between three drawing algorithms (example image output for each shown below):

**1. Spiral**  
**2. Shifting lines of images**  
**3. Custom path which changes direction depending which notes are played**

<br>
<br>


## 1. Spiral algorithm

![MallowSpiral](https://user-images.githubusercontent.com/21164488/166621617-1f49b3e6-6c8b-497f-a54a-0b312dd0e85a.PNG)
![savedSpiral](https://user-images.githubusercontent.com/21164488/158470652-236f165b-4780-4b85-8750-11c1b5a39316.png)
![spiral with color applied](https://user-images.githubusercontent.com/21164488/166621645-144c8c95-7e52-427c-ba28-a86b02edf6ab.PNG)


<br>

## 2. Shifting line segments algorithm

![savedLeaf](https://user-images.githubusercontent.com/21164488/158470676-e7656b5a-23a3-4fd9-a53b-937ac6638fa7.png)
![FallToWinter](https://user-images.githubusercontent.com/21164488/166621433-003fc733-64fb-4438-9789-5b2263b28b73.PNG)
![FallScribbles2](https://user-images.githubusercontent.com/21164488/166621508-1b4db954-0252-430d-ae41-3541f612f6a2.PNG)

<br>

## 3. Custom path algorithm

![savedForest](https://user-images.githubusercontent.com/21164488/158470692-dbd2fb12-0fde-47c8-9499-5f3c334c9055.png)

<br>
When algorithm finishes, a screenshot is captured and saved to an image file for user to do with as they please.
