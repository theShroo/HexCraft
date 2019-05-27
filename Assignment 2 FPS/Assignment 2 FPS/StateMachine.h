#ifndef STATEMACHINE_H
#define STATEMACHINE_H
#include "MathsHelper.h"
#include "SpriteManager.h"

template <typename _Enum, class _Ty>
class StateMachine {
public:
	// using pointers to member functions only is pretty silly, so im using generic pointers 
	// instead, these CAN point to member functions, but can also accept lambdas.
	typedef std::function<void()> CallbackOnEnter;
	typedef std::function<void(float)> CallbackOnUpdate;
	typedef std::function<void()>CallbackOnRender;
	typedef std::function<void()>CallbackOnExit;

private:
	_Ty* m_pOwner;
	_Enum m_currentState;

	// Maps to store function pointers to state enter/update/render/exit functions.
	std::unordered_map<_Enum, CallbackOnUpdate> m_statesOnUpdate;
	std::unordered_map<_Enum, CallbackOnRender> m_statesOnRender;
	std::unordered_map<_Enum, CallbackOnEnter> m_statesOnEnter;
	std::unordered_map<_Enum, CallbackOnExit> m_statesOnExit;
	// every state needs its own texture manager, so lets slave these into the template.
	std::unordered_map<_Enum, SpriteManager*> m_spriteManagers;

public:
	bool operator=(const _Enum& state) const{
		ChangeState(state);
		return 0;
	}

	StateMachine(_Ty* pOwner, _Enum emptyState)
	{
		m_currentState = emptyState;
		m_pOwner = pOwner;
	}

	~StateMachine()
	{
		std::unordered_map<_Enum, SpriteManager*>::iterator terminator;
		for (terminator = m_map.begin(); terminator != m_map.end(); terminator++) {
			if (terminator->second) {
				delete terminator->second;
				terminator->second = 0;
			}
		}
	}

	void RegisterState(_Enum state, CallbackOnEnter callbackEntry, CallbackOnUpdate callbackUpdate, CallbackOnRender callbackRender, CallbackOnExit callbackExit)
	{
		m_statesOnEnter[state] = callbackEntry;
		m_statesOnUpdate[state] = callbackUpdate;
		m_statesOnRender[state] = callbackRender;
		m_statesOnExit[state] = callbackExit;
		m_spriteManagers[state] = new SpriteManager();
	}

	_Enum GetCurrentState(void)
	{
		return m_currentState;
	}

	void ChangeState(_Enum statenext)
	{
		if (m_pOwner)
		{
			CallbackOnExit callbackExit = m_statesOnExit[m_currentState];
			if (callbackExit)
			{
				// Exit old state
				callbackExit();
			}
		}

		m_currentState = statenext;

		if (m_pOwner)
		{
			CallbackOnEnter callbackEnter = m_statesOnEnter[m_currentState];
			if (callbackEnter)
			{
				// Enter new state
				callbackEnter();
			}
		}
	}

	//Update Current State
	void Update(float fTimeStep)
	{
		if (m_pOwner)
		{
			m_spriteManagers[m_currentState]->Update(fTimeStep);
			CallbackOnUpdate callback = m_statesOnUpdate[m_currentState];
			if (callback){
				callback(fTimeStep);

			}
		}
	}

	//Render Current State
	void Render()
	{
		if (m_pOwner) {
			CallbackOnRender callback = m_statesOnRender[m_currentState];
			if (callback){
				callback();
			}
			m_spriteManagers[m_currentState]->Render();
		}
	}

	// access the spritemanager for a given state
	SpriteManager* Spritemanager(_Enum state) {
		if (m_pOwner) {
			if (m_spriteManagers.count(state) > 0) {
				return m_spriteManagers[state];
			}
			else { return 0; }
		}
		else { return 0; }
	}
};




#endif // !STATEMACHINE_H
