<h1 align="center">
Astro - A Smart Desk Lamp
</h1>
![Logo](/astro.png)
---
<p align="center">
This repository contains the Arduino code for Astro, a smart desk lamp designed and built by Peter Zhao, Audrey Wang, Joelie Simkins, and Henry Lin.
</p>
---
## Summary
The goal of this project was to create a smart desk lamp that could be effectively used by everyone, focusing specifically on those who might have dexterity issues. Along with this accessibility aspect, the lamp was also created with the goal of increasing productivity and focus, while decreasing its hindrance to sleep for its users. Addressing those with dexterity issues, the desk lamp’s light can be turned on using voice activation so that users don’t have to physically interact with the lamp to be able to make use of it. In the case that voice activation may not be desired, there is also a button, specifically chosen with a large diameter, that can be used to turn the light on/off as well as cycle through the given settings. The button’s large diameter ensures that users do not need fine motor skills to interact with it. To increase productivity and focus, the desk lamp has systems in place so that as the day progresses, the amount of blue light emissions decreases. The lamp also has the ability to, depending on the amount of light in a room, change light settings both automatically using a light sensor or manually using the button or voice module. This can allow the user to work for long periods of time without having to worry about straining their eyes. 

## Important Design Elements and Selection Basis
The choice behind our included materials was intentional and each component contributes uniquely to our final product. The specific neck design, utilization of gooseneck, was chosen to increase the ease needed to adjust the location of the light. Unlike typical lamps, the position of the on/off switch for the lamp is on the front so it can easily be accessed without having to reach around to the back of the lamp to use it. The light sensor allows the lamp to automatically adjust its brightness based on the amount of light in a given room so that eye strain may be decreased. The voice module was chosen so that users with dexterity issues would not have to physically interact with the lighting system to turn it on/off or change its settings. The lamp uses the time module to regulate the amount of blue light being emitted so that the focus, productivity, and sleep of users are not negatively impacted. The large button was chosen because it allows users to interact with it without having to worry about using fine motor skills as it has a large diameter. 

## Description of Final Product
Our final product takes in user input and lamp surroundings to output various brightnesses and colors throughout the day. Users can activate the lamp by saying “Astro” followed by a command, or by using the large red button. Alternatively, a user could do nothing and the lamp would actively change settings based on time and room brightness. It is powered by a 5V power supply, and automatically starts listening for its keywords once the on switch is flipped.

## Evaluation of Results
Upon the completion of our project, we can reasonably conclude the success of our efforts. Compared to our initial metrics, we tripled our desired luminosity, achieved 3 different RGB ratios to adjust brightness, and completely cut out blue light at night. By tripling the luminosity, we can now reach 5 square feet of a lamp with direct light and any subsequent square footage with cascading light (Appendix J), adequately lighting average-sized desks. Throughout the day, blue light varies from 100% to 40% to 0%, contributing to improved sleep quality and exceeding our initial goal of bringing blue light below 16% at night.
