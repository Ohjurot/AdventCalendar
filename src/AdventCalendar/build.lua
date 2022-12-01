advent_new_project("AdventCalendar", "src/AdventCalendar")
advent_exe()

debugdir "%{wks.location}/app"
debugargs { "-v", "-f", "24" }
