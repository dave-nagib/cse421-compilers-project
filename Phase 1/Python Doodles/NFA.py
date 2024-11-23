class NFA:

  def __init__(self):
    self.transitions = {} # 2D dictionary --- dictionary(state -> dictionary(input -> list(state)))
    self.states = set()
    self.initial_state = -1
    self.accepting_states = {} # dictionary(state -> pattern_id)


  # Can be overloaded with pattern_id to signify an accepting state without the boolean
  def add_state(self, state: int, initial: bool, accepting: bool, pattern_id: str|int) -> None:
    '''Add a state to the NFA. The state can be an initial and/or accepting. If it is accepting, a pattern_id must be provided.'''
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
      if ip in self.transitions[src]:
        self.transitions[src][ip].append(dst)
      else:
        self.transitions[src][ip] = [dst]
    else:
      self.transitions[src] = {ip:[dst]}


  def eps_closure(self, states: set) -> set: # Private method
    '''Returns the epsilon closure (all reachable states by empty transitions) of a set of states.'''
    assert all(state in self.states for state in states), 'States set contains (an) invalid state(s).'
    closure = set() | states
    stack = list(states)
    while stack:
      state = stack.pop()
      for dst in self.transitions.get(state, {}).get('eps', []):
        if dst not in closure:
          closure.add(dst)
          stack.append(dst)
    return closure



  def get_initial(self) -> set:
    '''Returns the initial state of the NFA.'''
    return self.eps_closure({self.initial_state})


  def get_accepting(self) -> dict:
    '''Returns a dictionary of accepting states and their corresponding pattern_ids.'''
    return self.accepting_states


  def get_single_accepting(self) -> int:
    '''Returns the accepting state of the NFA. Assumes that the NFA has only one accepting state.'''
    assert len(self.accepting_states) == 1, 'NFA has zero or more than one accepting states.'
    return list(self.accepting_states.keys())[0]
  

  def accept(self, state: int) -> str|int:
    '''Returns the pattern_id of a state if it is an accepting state and None otherwise.'''
    return self.accepting_states.get(state, None)


  def get_states(self) -> set:
    '''Returns the set of states in the NFA.'''
    return self.states


  def transition(self, states: set, ip: str) -> set:
    '''Transition function that returns a set of next states after getting a certain input while in a certain set of states.'''
    assert all(state in self.states for state in states), 'States set contains (an) invalid state(s).'
    dst = set()
    for state in states:
      dst.update(self.transitions.get(state, {}).get(ip, []))
    return self.eps_closure(dst)

  
  # STATIC IN THE CLASS OF NFA
  # Can be overloaded with two versions, one with a common accepting state and one without
  def union(nfas: list['NFA'], new_start: int, new_accept: int, pattern: str|int, common_accept: bool) -> 'NFA':
    '''Returns a new NFA that accepts the union of the languages of a list of NFAs. Corresponds to the '|' operator in regex.
    Can have a common accpeted lagnuage or differentiate between the accepted language of each input NFA.  
    **Assumes that all input NFAs have a single accepting state.**'''
    # Combine the states of all NFAs into a new set
    new_states = set()
    total_state_num = 0
    for nfa in nfas: 
      new_states |= nfa.states
      total_state_num += len(nfa.states)
    assert new_start not in new_states, 'New start state already exists in one of the NFAs.'
    assert len(new_states) == total_state_num, 'There is overlap between the states of the NFAs.'
    # Combine the transitions of all NFAs into a new set
    new_transitions = {}
    for nfa in nfas: new_transitions |= nfa.transitions
    # Construct the new NFA with combined states and transitions
    nfa = NFA()
    nfa.states = new_states
    nfa.transitions = new_transitions
    # Add a common start with empty transitions to the start states of all NFAs
    nfa.add_state(new_start, initial=True, accepting=False, pattern_id=None)
    for nfa in nfas: nfa.add_transition(new_start, nfa.initial_state, 'eps')
    # Choose a common accepting state or use multiple accepting states
    if common_accept:
      assert new_accept is not None, 'Common accepting state not provided.'
      assert pattern is not None, 'Pattern ID not provided.'
      assert new_accept not in new_states, 'New accepting state already exists in one of the NFAs.'
      nfa.add_state(new_accept, initial=False, accepting=True, pattern_id=pattern)
      for nfa in nfas: 
        nfa.add_transition(nfa.get_single_accepting(), new_accept, 'eps')
    else:
      for nfa in nfas: 
        nfa.add_state(nfa.get_single_accepting(), initial=False, accepting=True, pattern_id=nfa.accept(nfa.get_single_accepting()))
    return nfa


  def simple_union(self, other: 'NFA', new_start: int, new_accept: int, pattern: str|int) -> 'NFA':
    '''Returns a new NFA that accepts the union of the languages of this NFA and another NFA. Corresponds to the '|' operator in regex.    
    **Assumes that both NFAs have a single accepting state.**'''
    # Combine the states of both NFAs into a new set
    new_states = self.states | other.states
    assert new_start not in new_states, 'New start state already exists in one of the NFAs.'
    assert len(new_states) == len(self.states) + len(other.states), 'There is overlap between the states of the two NFAs.'
    # Combine the transitions of both NFAs into a new set 
    new_transitions = self.transitions | other.transitions
    # Get the starting and ending states of both NFAs
    self_start, other_start = self.initial_state, other.initial_state
    self_accept, other_accept = self.get_single_accepting(), other.get_single_accepting()
    # Construct the new NFA with combined states and transitions
    nfa = NFA()
    nfa.states = new_states
    nfa.transitions = new_transitions
    # Add a common start with two empty transitions
    nfa.add_state(new_start, initial=True, accepting=False, pattern_id=None)
    nfa.add_transition(new_start, self_start, 'eps')
    nfa.add_transition(new_start, other_start, 'eps')
    # Add a common accepting state as destination with empty transitions from both ends
    nfa.add_state(new_accept, initial=False, accepting=True, pattern_id=pattern)
    nfa.add_transition(self_accept, new_accept, 'eps')
    nfa.add_transition(other_accept, new_accept, 'eps')
    return nfa


  def simple_concat(self, other: 'NFA', pattern: str|int) -> 'NFA':
    '''Returns a new NFA that accepts the concatenation of the language of this NFA followed by another NFA's language. Corresponds to concatenation in regex.  
    **Assumes that both NFAs have a single accepting state.**'''
    # Combine the states of both NFAs into a new set
    new_states = self.states | other.states
    assert len(new_states) == len(self.states) + len(other.states), 'There is overlap between the states of the two NFAs.'
    # Combine the transitions of both NFAs into a new set 
    new_transitions = self.transitions | other.transitions
    # Get the start of this NFA and the accepting state of the other NFA
    self_start = self.initial_state
    other_accept = other.get_single_accepting()
    # Construct the new NFA with combined states and transitions
    nfa = NFA()
    nfa.states = new_states
    nfa.transitions = new_transitions
    # Set the common start to be this NFA's start
    nfa.add_state(self_start, initial=True, accepting=False, pattern_id=None)
    # Set the common accepting state as the other NFA's accepting state
    nfa.add_state(other_accept, initial=False, accepting=True, pattern_id=pattern)
    # Add an empty transition from this NFA's accepting state to the other NFA's start
    nfa.add_transition(self.get_single_accepting(), other.initial_state, 'eps') # TODO can be collapsed to one state
    return nfa
  

  def simple_repeat(self, new_accept: 'NFA', pattern: str|int, zero_or_more: bool) -> 'NFA':
    '''Returns a new NFA that accepts the Kleene or Positive closures of the language of this NFA. Corresponds to the '*' or '+' operators in regex.  
    **Assumes that this NFA has a single accepting state.**'''
    # Construct a new NFA with the same states and transitions
    nfa = NFA()
    nfa.states = self.states # TODO Deep copy
    nfa.transitions = self.transitions # TODO Deep copy
    # Add an empty transition from end to start (loop)
    nfa.add_transition(self.get_single_accepting(), self.initial_state, 'eps')
    # Set the start state as the old start state
    nfa.add_state(self.initial_state, initial=True, accepting=False, pattern_id=None)
    # Add a new accepting state with an empty transition from the old accepting state
    nfa.add_state(new_accept, initial=False, accepting=True, pattern_id=pattern)
    nfa.add_transition(self.get_single_accepting(), new_accept, 'eps')
    if zero_or_more:
      # Add an empty transition from the start to the new accepting state (skip the pattern)
      nfa.add_transition(self.initial_state, new_accept, 'eps')
    return nfa