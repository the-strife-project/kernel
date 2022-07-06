# Strife kernel drivers

These are drivers that are either:

- Used extremely often, so much that it would slow the system unnecesarily, such as APIC (maybe it's impossible to take it out).
- Used once at bootstrap, so it doesn't really matter where they are, such as ACPI and PIT.