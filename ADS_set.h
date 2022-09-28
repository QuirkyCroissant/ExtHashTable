#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

//Extendable Hash Table Project - Florian Hajek
//N = Bucket Size 
template <typename Key, size_t N = 12>
class ADS_set {
public:
  class Iterator;
  using value_type = Key;
  using key_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using const_iterator =  Iterator ;
  using iterator = const_iterator;
  //using key_compare = std::less<key_type>;        // B+-Tree
  using key_equal = std::equal_to<key_type>;        // Hashing
  using hasher = std::hash<key_type>;               // Hashing

  private:
    enum class Mode{free, used, free_again};
    struct Element{
      key_type key;
      Mode mode{Mode::free};
    };

    struct Bucket{
      Element values[N];
      size_type cnt_elements;
      size_type local_depth{0};
      //PH2 - Successor Bucket Pointer
      Bucket* successor_ptr = nullptr;

      Bucket(): cnt_elements{0}, local_depth{0} {}
      Bucket(size_type ld): cnt_elements{0}, local_depth{ld} {}
      /*Bucket(const Bucket& b){
        this->local_depth = b.local_depth; 
        this->cnt_elements = b.cnt_elements;
        for(size_type i{0}; i < cnt_elements;i++)
          this->values[i]=b.values[i];

        delete b;
      }*/

      ~Bucket() { /*delete[] this->values;*/ }
      
      bool AddBucketElem(const value_type& elem){
        if(cnt_elements < N){
                values[cnt_elements] = {elem, Mode::used};
                cnt_elements++;
                return true;
            }
            return false;
      }

      Element* FindValue(const value_type& elem){
          for(size_type i{0}; i < cnt_elements; i++)
            if(key_equal{}(values[i].key, elem))
              return &values[i];

          return NULL;

      }

      void ReshuffleElement(){
        
        // iterates through array and moves Mode::used Elements 
        // in front while switching them with Mode::free_again ones
        // if array consists out of only Mode::used Elements then Reshuffle does nothing

        //example:
        
        // cnt 0
        // 1x 2 3x 4 

        // cnt 1
        // 2 1x 3x 4

        // cnt 2
        // 2 4 3x 1x
        using std::swap;

        size_type cnt {0};
        for(size_type i{0}; i < N; i++){
          if((values+i)->mode == Mode::used){
            //this->values[cnt]=values[i];
            swap(this->values[cnt], values[i]);
            cnt++;
          }
        }

        this->cnt_elements = cnt;

      }

      // changes Attribute of given Element and than rearranges the Bucket
      void RemoveElem(Element* elem){
        elem->mode = Mode::free_again;
        this->ReshuffleElement();
      }

      size_type GetPosition(const value_type& elem){
        for(size_type i{0}; i < cnt_elements; i++)
            if(key_equal{}(values[i].key, elem))
              return i;

          return N;
      }
      
      
      void print(std::ostream& os){

        os << "countElems: " << cnt_elements << ", Local Depth: " << local_depth<< "\n";
        for (size_type i{0}; i < N; i++){
          if(values[i].mode == Mode::used)
            os << values[i].key << " ";
          else if(values[i].mode == Mode::free_again)
            os << "-" << values[i].key << "- ";
          else 
            os << "_ ";
        }
        
      }
      

    };

    /* Developer notes:

        -global_depth ... defines the directory table size( 2^global_depth )
    
        -directory_table ... saves all the pointer which point to the Bucket objects,
                            which hold their respective data
        -current_directory_size ... saves the actual table size of "directory_table"

        -current_size ... gives information, about how many elements are saved in the 
                          data structure atm. // maybe has to be altered into a FUNCTION later
    */
    size_type global_depth;
    Bucket** directory_table;
    size_type current_directory_size;
    size_type current_size;
    // PH2 - Iterator, this is the last Bucket Object in the list of objects (iterator loop starts with this bucket) 
    Bucket* head_bucket = nullptr;

    void add(const key_type &key);
    Element *locateElem(const key_type &key) const;
    size_type h(const key_type &key) const { return hasher{}(key) & ((1 << this->global_depth)-1); }
    
    //Collission Helper Functions
    void BucketOverflow(Bucket* b, size_type id);
    bool Split(Bucket* b, size_type id);
    void Directory_Expansion();

public:

  ADS_set() {
    this->global_depth = 0;
    this->directory_table = new Bucket*[1] { new Bucket(0) };
    this->current_directory_size = 1 << global_depth;
    this->current_size = 0;
    this->head_bucket = this->directory_table[0];
  }
  
  ADS_set(std::initializer_list<key_type> ilist): ADS_set{} { insert(ilist); }
  template<typename InputIt> ADS_set(InputIt first, InputIt last): ADS_set{} { insert(first, last); } 
  ADS_set(const ADS_set &other);

  ~ADS_set();

  ADS_set &operator=(const ADS_set &other);
  ADS_set &operator=(std::initializer_list<key_type> ilist);

  size_type size() const { return current_size; }
  bool empty() const { return (current_size == 0); }

  void insert(std::initializer_list<key_type> ilist){ insert(ilist.begin(), ilist.end()); } 
  std::pair<iterator, bool> insert(const key_type &key);
  //void insert(const key_type &key) { add(key); } //only for testing - PH1
  template<typename InputIt> void insert(InputIt first, InputIt last);

  void clear();
  size_type erase(const key_type &key);

  size_type count(const key_type &key) const { return (locateElem(key) != NULL); }                          // PH1
  iterator find(const key_type &key) const;

  void swap(ADS_set &other);

  const_iterator begin() const { return const_iterator{ this->head_bucket, 0}; }
  const_iterator end() const { return const_iterator{ nullptr, 0}; }

  void dump(std::ostream &o = std::cerr) const;

  friend bool operator==(const ADS_set &lhs, const ADS_set &rhs) {    
    if(lhs.size() != rhs.size()) return false;
      for(const auto &k: lhs) 
        if(!rhs.count(k)) return false;
    return true;
  }
  friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs){ return !(lhs == rhs); }
};

template <typename Key, size_t N>
ADS_set<Key,N>::ADS_set(const ADS_set &other) : ADS_set() {
    for(const auto& k : other) add(k);
}

template <typename Key, size_t N>
ADS_set<Key,N>::~ADS_set() { 
    
    while(this->head_bucket != nullptr){
      Bucket* tmp = this->head_bucket;
      this->head_bucket = this->head_bucket->successor_ptr;
      delete tmp;
    }
    
    // deletes table itself
    delete[] this->directory_table; 
    
}

//done
template <typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key,N>::operator=(const ADS_set &other){
  ADS_set tmp{other};
  swap(tmp);
  return *this;
}

//done
template <typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key,N>::operator=(std::initializer_list<key_type> ilist){
  ADS_set tmp{ilist};
  swap(tmp);
  return *this;
}

//done
template <typename Key, size_t N>
std::pair<typename ADS_set<Key,N>::iterator, bool> ADS_set<Key,N>::insert(const key_type &key){
  
  size_type dir {hasher{}(key) & ((1 << this->global_depth)-1)};

  //if the value already exists, then return the pair of it's iterator and false
  if(this->directory_table[dir]->FindValue(key))
    return {const_iterator{this->directory_table[dir], this->directory_table[dir]->GetPosition(key)}, false};
  else{
    add(key);
    dir = (hasher{}(key) & ((1 << this->global_depth)-1)); //<--- here was the mistake
    return {const_iterator{this->directory_table[dir], this->directory_table[dir]->GetPosition(key)}, true};
  }
}

// done
template <typename Key, size_t N>
void ADS_set<Key,N>::clear(){
  ADS_set tmp;
  swap(tmp);
}


// done
template <typename Key, size_t N>
typename ADS_set<Key,N>::size_type ADS_set<Key,N>::erase(const key_type &key){
  if(auto elem {locateElem(key)}){
    size_type id {h(key)};
    this->directory_table[id]->RemoveElem(elem);
    --this->current_size;
    return 1;
  }
  return 0;

}

// done
template <typename Key, size_t N>
typename ADS_set<Key,N>::iterator ADS_set<Key,N>::find(const key_type &key) const{

  size_type dir {h(key)};

  //if the value already exists, then return the pair of it's iterator and false
  if(this->directory_table[dir]->FindValue(key))
    return const_iterator{this->directory_table[dir], this->directory_table[dir]->GetPosition(key)};
  else
    return this->end();
}

// done
template <typename Key, size_t N>
void ADS_set<Key,N>::swap(ADS_set &other){
  using std::swap;
  swap(this->global_depth, other.global_depth);
  swap(this->directory_table, other.directory_table);
  swap(this->current_directory_size, other.current_directory_size);
  swap(this->current_size, other.current_size);
  swap(this->head_bucket, other.head_bucket);
  
}

//Below: PH1

// calls add function with each input value
template <typename Key, size_t N>
template<typename InputIt> void ADS_set<Key,N>::insert(InputIt first, InputIt last){
    for(auto it{first}; it != last; ++it){
        add(*it);
    }
}


template <typename Key, size_t N>
void ADS_set<Key,N>::add(const key_type &key){
    size_type dir_id {h(key)};

    //if Value already exists, then cancel the function call
    if(this->directory_table[dir_id]->FindValue(key) != NULL)
      return;

    //adding value into the structure, if not possible call Collision Handler
    if(this->directory_table[dir_id]->AddBucketElem(key))
      this->current_size++;
    else{
      BucketOverflow(directory_table[dir_id], dir_id);
      add(key);
    }

}

//"Collision Handler" for times, when insertion wasn't successful because of an Overflow 
template <typename Key, size_t N>
void ADS_set<Key,N>::BucketOverflow(Bucket* b, size_type id){
  // After unsuccessfull Insertion into the table, decides 
  // whether to split or expand the table 
  if(b->local_depth < this->global_depth)
    Split(b, id % ((size_type)1 << b->local_depth));
  else
    Directory_Expansion();

}


template <typename Key, size_t N>
bool ADS_set<Key,N>::Split(Bucket* bucket_to_split, size_type dir_id){
  
  // creates buffer Bucket
  Bucket* new_bucket{new Bucket(++(bucket_to_split->local_depth))};

  for(size_type idx{0}; idx < bucket_to_split->cnt_elements; idx++){
    if(bucket_to_split->values[idx].mode == Mode::used){
    
      value_type elem {bucket_to_split->values[idx].key};
      size_type local_hash = hasher{} (elem) & (((size_type)1 << (bucket_to_split->local_depth))-1);

      //if value has to be rehashed, add it to the new bucket and mark it as "free_again" in old
      if(local_hash != dir_id){
        new_bucket->AddBucketElem(elem);
        bucket_to_split->values[idx].mode = Mode::free_again;
      }
    }
  }

  // after rehashed values are added to new bucket, the old one has to be sorted again
  // so that we can add values again later
  bucket_to_split->ReshuffleElement();

  //Calculates on which directory_table_entry the new Bucket has to be placed
  const size_type new_adress {dir_id+(1<<(bucket_to_split->local_depth-1))};
  this->directory_table[new_adress] = new_bucket;

  // PH2 - successor-pointer of new bucket becomes gets the nullpointer from 
  // head bucket but also becomes the new Head
  new_bucket->successor_ptr = this->head_bucket;
  this->head_bucket = new_bucket;
  
  // if their are multiple pointers on the new Bucket, than they will also be refreshed
  for(size_type i{new_adress}; i < this->current_directory_size; i = i + (1<< bucket_to_split->local_depth))
    this->directory_table[i] = new_bucket;

  return true;
  
}


template <typename Key, size_t N>
void ADS_set<Key,N>::Directory_Expansion(){

  this->global_depth++;
  const size_type new_directory_size {this->current_directory_size << 1};
  Bucket** temp = new Bucket*[new_directory_size];

  for(size_type idx{0}; idx < this->current_directory_size; ++idx){
    temp[idx] = this->directory_table[idx];
    temp[idx+this->current_directory_size] = this->directory_table[idx];
  
  }
  
  delete[] this->directory_table;
  this->directory_table = temp;
  
  this->current_directory_size = new_directory_size;

}

//looks up value in retrospective the hash table entry
template <typename Key, size_t N>
typename ADS_set<Key,N>::Element *ADS_set<Key,N>::locateElem(const key_type &key) const{
    
    size_type dir_id{h(key)};
    return (directory_table[dir_id]->FindValue(key));
        
}



template <typename Key, size_t N>
void ADS_set<Key,N>::dump(std::ostream &os) const{
    os << "---------- Hashtable Data ----------\n";
    os << "global depth = "<< global_depth <<" directory_size = " << current_directory_size << ", current_size = " << current_size << '\n' << '\n';
    
     for(size_type idx{0}; idx < current_directory_size; ++idx){
        os << "directory index "<< idx << "("<< &directory_table[idx] << ")"<< ":\n";
        if(directory_table[idx] != nullptr){
            directory_table[idx]->print(os);
            
            if(directory_table[idx]->successor_ptr)
              os << "\t|-> Bucket #" << h(directory_table[idx]->successor_ptr->values[0].key);
            else 
              os << "\t|-> NULL";
            
            os << "\n";
        }
        os << '\n';
        
    }
    os << "------------------------------------\n";
    os << '\n';

    this->head_bucket->print(os); 
    os << "\n"; 
    
    /*  os << "pointercheck:\n";
    for(size_type buck_a{0}; buck_a < current_directory_size; ++buck_a){    
        for(size_type buck_b{0}; buck_b < current_directory_size; ++buck_b){
            if(directory_table[buck_a]==directory_table[buck_b] && buck_a != buck_b)
                os << "Bucket #" << buck_a << " == Bucket #" << buck_b << '\n';
        }
        os << "\n";
    } */

    for(auto i =this->begin(); i != this->end(); ++i){
      os << *i << " ";
    }
    os << "\n";
     

}


/*
  Iterator Style is implemented as a Linked List out of all the Buckets eg.:
  (Head-)Bucket1->Bucket2->Bucket3->NULL

  For this we need a new variable in the Bucket Struct that points to its new successor
  while we have an additional counter variable in the Iterator which points to the current element.
*/
template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
public:
  using value_type = Key;
  using difference_type = std::ptrdiff_t;
  using reference = const value_type &;
  using pointer = const value_type *;
  using iterator_category = std::forward_iterator_tag;
private:
  Bucket* curr_bucket_ptr;
  size_type curr_elem;
  
  void skip(){
    //iterates through Bucket and if thats done, we change the bucketptr to its successor
    // if there is none, than we reached the end-Iterator and we cancel the function
    
      while(curr_bucket_ptr != nullptr && this->curr_bucket_ptr->values[curr_elem].mode != Mode::used) {
        switch (this->curr_elem < this->curr_bucket_ptr->cnt_elements) {
        case true:
          this->curr_elem++;
          break;
        
        default:
          this->curr_bucket_ptr = curr_bucket_ptr->successor_ptr;
          this->curr_elem = 0;
          break;
        }
        
        
        /* if(this->curr_elem < this->curr_bucket_ptr->cnt_elements)
          this->curr_elem++;
        else {
          //if(this->curr_bucket_ptr != NULL && this->curr_bucket_ptr->successor_ptr != NULL){
          this->curr_bucket_ptr = curr_bucket_ptr->successor_ptr;
          this->curr_elem = 0;
          
        } */

      }
    

  }

public:
  explicit Iterator (Bucket* b = nullptr, size_type e=0): curr_bucket_ptr{b}, curr_elem{e} { skip(); };
  reference operator*() const { return this->curr_bucket_ptr->values[this->curr_elem].key; }
  pointer operator->() const { return &this->curr_bucket_ptr->values[this->curr_elem].key; }
  
  // if there is still space in the Bucket than curr_element index will be incremented
  // otherwise, if still possible Bucket pointer will be altered
  Iterator &operator++() { 
    if(curr_elem < this->curr_bucket_ptr->cnt_elements)
      curr_elem++;
    else if(this->curr_bucket_ptr->successor_ptr != NULL){
      this->curr_bucket_ptr = curr_bucket_ptr->successor_ptr;
      this->curr_elem = 0;
    }  
    skip(); 
    return *this; 
    
  }
  Iterator operator++(int) { auto temp{*this}; ++*this; return temp; }
  
  friend bool operator==(const  Iterator  &lhs, const  Iterator  &rhs) { return ((lhs.curr_bucket_ptr == rhs.curr_bucket_ptr) && (lhs.curr_elem == rhs.curr_elem));}
  friend bool operator!=(const  Iterator  &lhs, const  Iterator  &rhs) { return !(lhs == rhs); }
};

template <typename Key, size_t N>
void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }


#endif // ADS_SET_H
