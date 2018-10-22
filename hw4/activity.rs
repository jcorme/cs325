use std::io::{BufRead, BufReader};
use std::str::SplitWhitespace;
use std::fs::File;

#[derive(Debug)]
struct Activity {
    number: i32,
    start_time: i32,
    end_time: i32,
}

impl<'a> From<SplitWhitespace<'a>> for Activity {
    fn from(s: SplitWhitespace<'a>) -> Self {
        let activity_params: Vec<i32> = s
            .map(|x| x.parse().expect("Unable to parse activity parameter."))
            .collect();

        if activity_params.len() != 3 {
            panic!(format!("Expected 3 parameters for activity, received {}", activity_params.len()));
        }

        Activity {
            number: activity_params[0],
            start_time: activity_params[1],
            end_time: activity_params[2],
        }
    }
}

impl Activity {
    fn read_activities(mut stream: impl Iterator<Item=String>) -> Option<Vec<Activity>> {
        let num_activities: i32 = stream
            .next()?
            .parse()
            .expect("Unable to parse the number of activities into integer.");

        let activities: Vec<Activity> = (0..num_activities)
            .map(|_| stream.next().expect("Failed to read an activity line."))
            .map(|line| line.split_whitespace().into())
            .collect();

        Some(activities)
    }
}

fn read_input_file(name: &'static str) -> Vec<Vec<Activity>> {
    let f = File::open(name).expect(&format!("Unable to open input file {}", name));
    let buf = BufReader::new(f);
    let mut stream = buf
        .lines()
        .map(|s| s.expect("Unable to unwrap string in stream."))
        .filter(|s| s.trim().len() != 0);
    let mut activity_sets = Vec::new();

    loop {
        match Activity::read_activities(stream.by_ref()) {
            Some(activities) => activity_sets.push(activities),
            None => break,
        }
    }

    activity_sets
}

fn select_activities(mut activities: Vec<Activity>) -> Vec<Activity> {
    activities.sort_by(|a, b| b.start_time.cmp(&a.start_time));
    activities.into_iter().fold((Vec::new(), 999999999), |(mut acc, mut last_start_time), a| {
        if a.end_time <= last_start_time {
            last_start_time = a.start_time;
            acc.push(a);
        }

        (acc, last_start_time)
    }).0
}

fn main() {
    let mut set_number = 1;

    for set in read_input_file("act.txt") {
        let optimal_set = select_activities(set);

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
