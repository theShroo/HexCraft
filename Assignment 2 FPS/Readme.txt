IMPORTANT-READ FIRST:	 build and compile a realease version, the debug version lags horrendously. make sure the target platform is supported on your system, my IDE defaults to windows 10.1, yours may vary.
						 if the build doesnt work for whatever reason then there is a prebuilt release version stored in the release folder. it ran on my macahine, so i hope it works on your windows version.

Supermine! game. 
By Russell Chidambaranathan

Version 0.1.11:
Features:
	General:
		- Updated patch notes!
		- new block type - cobblestone. takes more damage before it breaks, look for it underground.
	Under the hood:
		- 3D mapping system!
		- State manager handles all the game stuff so that only the required elements are updated and rendered each frame.
		
	Engine features:
		- blocks can be broken (but not yet placed)
		- colisions and standing physics have been optimised to improve gameplay and performance
		- broken blocks will drop and can be collected
		- the render and update distances are now dynamic, as you move around more stuff will be loaded and unloaded, depending on your system specs. 
			- this can get memory intense, memory profiling shows memory usage up at 2gb after a 5 minutes, so memory optimisations will need to be implemented next, probably by defining chunks, 
				and pre calculating render and update lists per chunk, loading and unloading chunks in a spiral around the player, and iterating over chunk update and render lists
		- simplex noise for terrain height generation.
		
	Bugs fixed:
		- collecting items required breaking the block below the item and falling through it to trigger collision. - fixed.
		- collecting blocks resulted in a blank item in the inventory - Fixed.
		- when climing/jumping layers of the world wouldn't render correctly - traced to invalid check in movement update.
		- when exiting the program it failed to terminate properly - the window shutdown procedure wasn't being called correctly.
		- dropped blocks now correctly show the block type as items.
		- mouse no longer leaves the screen during gameplay.
		- stage one performance update has been implemented by precalculating render and update lists and updating those lists on player movement and render distance incrementation.
				- also here the neigbouring cells are precalculated for each cell and stored as an array of pointers in each cell to reduce the number of hash calls and give us faster access for testing
				this has resulted in aproximately 150% performance improvement, and increased render distance to more than double. more work is planned but will not be ready for this assignment deadline.
				this update has reduced the time cost of the hash compare function from 17% of total cpu to 0.02% a significant enough reduction that this "bug" is considered resolved.
				- baseline render distance at this stage has been marked at aproximately 40 tiles, while maintaining 60fps.
		- there is a render bug where sometimes breaking blocks will not render a newly exposed block, this has something to do with the calculations for checking exposed blocks, 
			but at this stage its a very minor bug that doesnt affect gameplay badly enough that its a priority yet. its easy to fix by jumping while near the block. - FIXED
		- bug where the player would lock up during gameplay has been fixed, traced back to a logic error in the Hex {operator-(){}} code resulting in a bad w value.
	known Bugs:
		- memory is poorly optimised, while there are no(or undetectable) memory leaks at this stage, the memory usage of all the cells is HUGE. converting cells into: a pointer to a type,
			neigbours and a position, with all methods moved to the type class would potentially reduce these overheads, and would bring the renderer one step closer to a rendering optimisation,
			which is currently the largest cpu overhead in the program, but at this stage is too much work to be ready before the deadline.
		- buttons loose the accuracy of the cursor when the window is resized during runtime, and their mouseover positions remain at a fixed position from the top left corner of the screen while the
			buttons sprite moves to remain at a relative position to the size of the window.
			
	notes:
		- using unordered_map is very powerful, and early this provided us with an immense advantage, 
			but as the number of calls to the map grow the performance of the hashing function is becoming a problem.
			there isnt much that can be done at this stage of development, and about 20 hours went into trying to reduce the number of map calls
			but the end result was a complete failure. the game has been designed with a borderless environment from the start and any attempt
			to limit the number of calls to a map require limiting the environment with either floor/ceiling, or horizontal limits along the x/z plane.
			but the way the system is configured it keeps trying to make calls outside those limits at almost every point in the code and it causes 
			crittical errors, usually trying to call member functions on null/invalid cell pointers. trying to perform checks to ensure that the 
			cell is within the limits ends up costing more than just runnning the hash function.
			
			to improve performance direct lookup without a hash function would be required, one solution would be by using a regular map we could remove 
			the hash function but this would reduce the time complexity of the lookup from O(1) + hash to 0(logN), with an n value in the range of 30000
			this would become even less appealing. 
			
			another would be to use vectors, but this would require reworking the code so that it filled in all
			the gaps in a map, and would require all position values to be unsigned, quite a feat of coding, which could be done, but would require a 
			complete rework of the engine.
			
			finally the fastest way would be to use fixed arrays, each array forming a chunk and each chunk containing a column of cells, with multiple
			chunks stacked atop each other to maintain limitless vertical space, this method would also require a rework, although less complete, would 
			still use a map for chunk lookup but reduce the number of lookups to 1/ [render/update distance]
			
			a this stage all these options look nice, but will require too much work to be ready before the deadline.
			
			ps. all these options have been discarded in favour of cheating the table by storing prechecked lists of cells that meet rendering and updating requirements, alleviating the load on the map structure.
			
			
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
version 0.1.7:
features:
	under the hood:
		-dependancy on SimpleMath.h has been removed:
			-all references to simplemath objects and functions have been replaced with native DirectX objects and functions.
			-some data sets had to be changed to adapt to the new data objects:
				-colour(float, float, float) has to be replaced with XMloadfloat4(float, float, float, float) 
					as directX functions that accept colour values read 4 values (rgbA) from the XMVECTOR object.
			-Colisions required serious rework.
				-values are stored and passed as XMFLOAT3, because access to member values is required, but transformed 
					as XMVECTORS since XMFLOAT3 has no operators defined, and no functions accept XMFLOAT3 as arguments.
		-map objects:
			-Map objects handle collisions to optimise collision logic.
			-map objects handle rendering optimisations
			-map classes and structs seperated into seperate files.		
			-map class handles entity creation and destruction to avoid corrupting the update process.
		-Aditional texture type supported: Targa.
		-Release compliles are supported.
		-required DirectXTK files have been added to the solution file for inspection and release compile.
	engine features:
		-fps player with camera fixed to player.\
			- health and attacking implemented.
			- ammo and weapons, with damage modifiers have been implemented through the equipment manager.
		-primitive ai implemeted (basic follow and fire).
		-physics based movement implemented.
		-flying camera.
		-debug output pane (toggle with F3)
		-texture manager.
		-mesh manager.
		-shader manager.
		-Sprite Manager:
			-stores complete sprite layouts for the purposes of switching menus and other overlays rapidly and easily.
			-handles its own rendering and updates.
		=> bullet manager to handle projectiles
			- bullets can be instatiated by any object and will inject themselves into the maps entity handler.
			- bullets that dont hit anything will eventually "time out" and be destroyed to prevent memory leaks.
			- bullets dont collide with their "shooter" until after they have left the gun. it is possible to shoot 
				yourself by outrunning your own bullets.
		- equipment manager
			-equipment template lookup
			-equipment comparison handler
			-inventory and equipped items handlers
		- collision management:
			-loot collection.
			-bullet impact detection. (uses rays to check for collisions that might happen between frames of fast traveling bullets.)
			-physical obstruction detection.
			

			
			
changelog:
cleaned up all type conversion errors.
-crosshair added.

- maps update function made more robust by invalidating the pointer if the target is scheduled for removal. 
	this means that the object cant invalidate the removal by updating istelf into a different cell.

notes::
	-typedef pair is so useful.
	-hashing pointers and 	operator PointerKey() { return PointerKey{ reinterpret_cast<size_t>(this) }; } lets you index items into a map
		by using the object itself as the key. incredibly handy in entity management and comparing one obbect to another for equality.
	-the bug with the bullets reomving themselves was traced back to poor detection of removal.
	- a bug with bullets hitting the player as they were fired was traced back to the calculation of the rays, 
		it was using the full length of the velocity vector, without adjusting for timestep.
		ideally this would have been done from start to end point, BUT the update call couldnt be made within the collision detection 
		loop without invalidating several iterators and so a calculation to get the final destination point was made before the actual update took place.
	
	- TLDR: rays cause problems.
	

--------------------------------------
Version 0.2.0

updated version to VS 2019, and modified the settings and dependancies to make it work.