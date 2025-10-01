# APPLICATION NOTE DOCS

    This directory will hold the application notes built for this project by each team, where each other team will be able to access information and understand
    how the specifics part of the project is being handled without the necessity of going through the datasheets, avoiding redundance and making it possible for
    each team to focus in their respective areas of the project. The documentaton will be divided in the 4 folders:

    - game
    - grid
    - programmger
    - protocols

# game
    
    Regarding the game task.

# grid

    Regargind the grid task

# programmer

    Regarding the programmer task

# protocols

    Use this directory to document any protocol you are working with. If you are doing any function that will handle a specific protocol, 
    use this to explain the source of your information, and the flow of how it should work and how to test it.






# Example of how to write your documentation: (I will do one example for the SPI protocol.)
# ##################################################################### #

# [TOPIC] APPLICATION NOTE

**Author(s):** [Team or individual name]  
**Date:** [MM/DD/YYYY]  
**Related Module:** [game | grid | programmer | protocols]

---

## Purpose

This document explains how **[specific topic, e.g., SPI, SRAM, LED timing]** is handled in this project.  
It is intended to provide a clear reference for other teams so they can understand the design decisions, configuration, and testing methods without having to parse datasheets directly.

---

## Scope

- Define what this note covers and what it does not.  
- Mention which part of the system depends on it (e.g., external memory, programmer, LED grid).  

---

## Configuration / Design

- Hardware settings (registers, pins, wiring, timing).  
- Software setup (initialization sequence, routines, algorithms).  
- Any assumptions or constraints.  

---

## Implementation Flow

Step-by-step description of how this feature is implemented:  
1. Initialization  
2. Data flow / command handling  
3. Error handling  
4. Interaction with other modules  

---

## Testing & Validation

- Recommended test methods (logic analyzer, oscilloscope, UART logs, etc.).  
- Expected outcomes
- Known limitations

---

## References

- Datasheets used  
- External application notes or guides  
- Internal cross-references (e.g., link to related AN docs in this repo)

---

## Revision History

| Date       | Version | Author     | Notes/Changes |
|------------|---------|------------|---------------|
| MM/DD/YYYY | v0.1    | [Name]     | Initial draft |
| MM/DD/YYYY | v0.2    | [Name]     | Updates/fixes |