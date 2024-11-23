class DFA:

  def __init__(self):
    self.transitions = {} # 2D dictionary --- dictionary(state -> dictionary(input -> state))
    self.states = set()
    self.initial_state = -1
    self.accepting_states = {} # dictionary(state -> pattern_id)


  def add_state(self, state: int, initial: bool, accepting: bool, pattern_id: str|int) -> None:
    '''Add a state to the DFA. The state can be an initial and/or accepting. If it is accepting, a pattern_id must be provided.'''
    assert state > -1, 'State ID is not a positive integer.'
    self.states.add(state)
    if initial: self.initial_state = state
    if accepting:
      assert pattern_id is not None, 'Pattern ID not provided for an accepting state.'
      self.accepting_states[state] = pattern_id


  def add_transition(self, src: int, dst: int, ip: str|set) -> None:
    '''Add a transition from src to dst on input ip.'''
    assert src in self.states and dst in self.states, 'Invalid source or destination states.'
    if src in self.transitions:
      self.transitions[src][ip] = dst
    else:
      self.transitions[src] = {ip:dst}


  def add_transitions(self, src: int, transitions: dict) -> None:
    '''Add all transitions from src at once with all possible inputs. The transitions dictionary should be of the form {input:dst}.'''
    assert src in self.states, 'Invalid source state.'
    for dst in transitions.values():
      assert dst in self.states
    self.transitions[src] = transitions


  def get_initial(self) -> int:
    '''Returns the initial state of the DFA.'''
    return self.initial_state


  def get_accepting(self) -> dict:
    '''Returns a dictionary of accepting states and their corresponding pattern_ids.'''
    return self.accepting_states


  def accept(self, state: int) -> str|int:
    '''Returns the pattern_id of a state if it is an accepting state and None otherwise.'''
    return self.accepting_states.get(state, None)
  

  def get_states(self) -> set:
    '''Returns the set of states in the DFA.'''
    return self.states


  def transition(self, state: int, ip: str) -> int:
    '''Transition function that returns the next state after getting a certain input while in a certain state.'''
    assert state in self.states, 'Invalid source state.'
    return self.transitions.get[state][ip]


  def validate(self, input_domain: set) -> bool:
    '''Validate that every state has a transition for every input in a given input domain.'''
    for state in self.states:
      if state not in self.transitions: return False
      for ip in input_domain:
        if ip not in self.transitions[state]: return False
    return True