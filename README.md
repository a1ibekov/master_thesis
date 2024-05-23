# Autonomous Planetary Soft-Landing Simulation Framework

## Overview

This repository presents an onboard perception-assisted high-fidelity simulation framework designed for autonomous planetary soft-landing. This framework integrates visual information processing with advanced onboard guidance systems within realistic simulation environments. Utilizing the open-source Unity game engine and the Robotic Operating System (ROS2), we emulate the motion of a spacecraft approaching planetary terrain with high precision.

## Features

- **High-Fidelity Simulation Environment**: Leveraging Unity for realistic 3D terrain modeling and spacecraft dynamics.
- **Physics-Based Simulation**: Incorporates a physics-based simulation toolbox for accurate emulation of spacecraft motion.
- **Onboard Camera Integration**: Equipped with an onboard camera to capture detailed local terrain images.
- **6-DOF Motion Capture**: Accurately simulates the spacecraft’s six degrees of freedom motion, including orientation and camera feed integration.
- **Realistic Illumination**: Includes a light source representing the Sun to simulate various illumination conditions.
- **Advanced Guidance Algorithms**: Implements an attitude-constrained minimum jerk guidance algorithm to emulate spacecraft motion.
- **Visual Odometer**: Processes the visual feed from the onboard RGB camera through a gradient extraction-based perception system to provide visual odometry.
- **Safe Landing Site Identification**: Identifies safe landing sites to avoid potential risks of landing on uneven terrain.

## Installation

1. **Prerequisites**:
   - Unity Game Engine
   - ROS2
   - [Other dependencies if any]

2. **Clone the Repository**:
   ```bash
   git clone https://github.com/yourusername/planetary-landing-simulation.git
   cd planetary-landing-simulation
