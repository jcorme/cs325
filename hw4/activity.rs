use std::i32;
use std::io::{BufRead, BufReader};
use std::str::SplitWhitespace;
use std::fs::File;

// Data structure representing an activity, with the paramters given in the input file
#[derive(Debug)]
struct Activity {
    number: i32,
    start_time: i32,
    end_time: i32,
}

// Converts a SplitWhitespace into an Activity
// Useful for reading the input file line by line; if a line holds an activity, we can create
// a SplitWhitespace and convert it into an Activity using two function calls
// e.g. line.split_whitespace().into()
impl<'a> From<SplitWhitespace<'a>> for Activity {
    fn from(s: SplitWhitespace<'a>) -> Self {
        // Tries to parse each number on the activity line into i32 values
        let activity_params: Vec<i32> = s
            .map(|x| x.parse().expect("Unable to parse activity parameter."))
            .collect();

        // Every activity should have three parameters: number, start_time, and end_time
        // Crash the program if anything other than three integer parameters is found
        if activity_params.len() != 3 {
            panic!(format!("Expected 3 parameters for activity, received {}", activity_params.len()));
        }

        // Construct the Activity
        // The order of parameters in the input file should always be number, start_time, and
        // end_time, which is why the indices are hardcoded as they are
        Activity {
            number: activity_params[0],
            start_time: activity_params[1],
            end_time: activity_params[2],
        }
    }
}

impl Activity {
    // Reads line by line from the input file, trying to parse an activity set
    // Returns None if no activity set could be found/parsed
    fn read_activity_set(mut stream: impl Iterator<Item=String>) -> Option<Vec<Activity>> {
        // The first line of an activity set should be the number of activities in the set
        // Try to parse that number out, otherwise crash
        let num_activities: i32 = stream
            .next()?
            .parse()
            .expect("Unable to parse the number of activities into integer.");

        // Read the next num_activities lines
        // Turn each line into an Activity by splitting at whitespace characters and calling
        // the From<SplitWhitespace> implemention of Activity
        let activities: Vec<Activity> = (0..num_activities)
            .map(|_| stream.next().expect("Failed to read an activity line."))
            .map(|line| line.split_whitespace().into())
            .collect();

        Some(activities)
    }
}

// Parses an input file into activity sets, returning a list of sets
fn read_input_file(name: &'static str) -> Vec<Vec<Activity>> {
    // Try to open the input file (act.txt)
    let f = File::open(name).expect(&format!("Unable to open input file {}", name));
    // Use BufReader to buffer reads from the file
    let buf = BufReader::new(f);
    // Read the file line-by-line and remove any lines that are just whitespace
    let mut stream = buf
        .lines()
        .map(|s| s.expect("Unable to unwrap string in stream."))
        .filter(|s| s.trim().len() != 0);
    // Holds the parsed activity sets
    let mut activity_sets = Vec::new();

    // It's unknown how many activity sets there are, so loop forever until read_activity_set
    // fails to parse a set
    loop {
        match Activity::read_activity_set(stream.by_ref()) {
            Some(activities) => activity_sets.push(activities),
            None => break,
        }
    }

    activity_sets
}

// Chooses the maximum-size subset of mutually compatible activities by selecting the last
// activity to start that is compatible with all previously chosen activities
fn select_activities(mut activities: Vec<Activity>) -> Vec<Activity> {
    // Sort the activities by descending start time (latest start time is first)
    // Vec::sort(_by) is an implementation of merge sort that runs in O(n*logn)
    activities.sort_by(|a, b| b.start_time.cmp(&a.start_time));

    // Holds the start time of the last selected activity
    // i32::MAX is a sentinel value so the first iteration's end time is always less (assuming
    // no start time is actually equal to this large number)
    let mut last_start_time = i32::MAX;
    // Holds the activities chosen by the algorithm
    let mut selected_activities = Vec::new();

    // Iterate through all of the activities
    for a in activities.into_iter() {
        // Is the activity compatible?
        if a.end_time <= last_start_time {
            // If the activity is compatible, its start time becomes the new last_start_time
            last_start_time = a.start_time;
            selected_activities.push(a);
        }
    }

    selected_activities
}

fn main() {
    // Counter to print the set number
    let mut set_number = 1;

    // Iterate through each activity set
    for set in read_input_file("act.txt") {
        // Find the maximum-size subset using the algorithm
        let optimal_set = select_activities(set);

        // Prints results with format shown in the homework
        println!("Set {}", set_number);
        println!("Number of activities selected = {}", optimal_set.len());
        print!("Activities:");

        for a in optimal_set.iter().rev() {
            print!(" {}", a.number);
        }

        println!("\n");

        set_number += 1;
    }
}
