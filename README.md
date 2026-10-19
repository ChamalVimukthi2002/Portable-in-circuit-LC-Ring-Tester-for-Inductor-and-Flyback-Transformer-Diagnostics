# Portable-in-circuit-LC-Ring-Tester-for-Inductor-and-Flyback-Transformer-Diagnostics
A key design feature is its “in-circuit testing capability”, using a low-amplitude test signal of approximately 100 mV across the component under test. This approach is intended to enable testing without removing the component from the PCB, Less subject to surrounding circuit conditions and possible interference from connected components.

The tester excites the component to produce a damped oscillation in an LC resonant circuit. A high-speed TLV3501 comparator detects the oscillation transitions, while a WCH CH552 microcontroller counts the resulting pulses to assess the oscillation's damping characteristics.

Key Features and Technical Work
- Designed and built a portable in-circuit inductive component tester.
- Developed a low-amplitude LC ring-testing circuit with an approximately 100 mV test signal.
- Integrated a TLV3501 high-speed comparator (4.5nS propagation delay) for oscillation detection.
- Developed embedded firmware for pulse counting using the WCH CH552 microcontroller.
- Applied ring-count analysis to evaluate damping characteristics and identify potential shorted turns or excessive inductive losses.

Limitations: this tester cannot be used to test Iron core transformers as losses are very high. It needs some modifications to be able to work on iron core transformers. 

How Testing Works: Ring count provides an indication of oscillation damping. Tester shows very low single digit counts or just 0 if there any shorted turns of coil (rings) is inside the Transformer or inductor as it saps away the energy from the LC tank circuit. Higher counts mean no rings.
